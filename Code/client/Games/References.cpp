#include <TiltedOnlinePCH.h>

#include <Games/References.h>

#include <RTTI.h>

#include <Forms/TESWorldSpace.h>
#include <Forms/TESObjectCELL.h>
#include <Forms/BGSHeadPart.h>
#include <Forms/TESNPC.h>
#include <Forms/TESPackage.h>
#include <Forms/TESWeather.h>
#include <SaveLoad.h>
#include <ExtraData/ExtraDataList.h>

#include <BSAnimationGraphManager.h>
#include <Misc/GameVM.h>
#include <Havok/BShkbAnimationGraph.h>
#include <Havok/hkbBehaviorGraph.h>
#include <Havok/hkbVariableValueSet.h>
#include <Havok/hkbStateMachine.h>

#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/AnimationVariables.h>

#include <Games/TES.h>
#include <Games/Overrides.h>
#include <Games/Misc/Lock.h>
#include <AI/AIProcess.h>
#include <Magic/MagicCaster.h>
#include <Sky/Sky.h>

#include <Events/LockChangeEvent.h>
#include <Events/InitPackageEvent.h>

#include <TiltedCore/Serialization.hpp>

#include <Services/PapyrusService.h>
#include <Services/DebugService.h>
#include <World.h>

#include <Combat/CombatController.h>
#include <AI/AITimer.h>
#include <Combat/CombatTargetSelector.h>

extern const AnimationGraphDescriptor* BehaviorVarPatch(BSAnimationGraphManager* pManager, Actor* pActor);

using ScopedReferencesOverride = ScopedOverride<TESObjectREFR>;
thread_local uint32_t ScopedReferencesOverride::s_refCount = 0;

TP_THIS_FUNCTION(TSetPosition, char, Actor, NiPoint3& acPosition);
TP_THIS_FUNCTION(TRotate, void, TESObjectREFR, float aAngle);
TP_THIS_FUNCTION(TActorProcess, char, Actor, float aValue);
TP_THIS_FUNCTION(TLockChange, void, TESObjectREFR);
TP_THIS_FUNCTION(TSetWeather, void, Sky, TESWeather* apWeather, bool abOverride, bool abAccelerate);
TP_THIS_FUNCTION(TForceWeather, void, Sky, TESWeather* apWeather, bool abOverride);

static TSetPosition* RealSetPosition = nullptr;
static TRotate* RealRotateX = nullptr;
static TRotate* RealRotateY = nullptr;
static TRotate* RealRotateZ = nullptr;
static TActorProcess* RealActorProcess = nullptr;
static TLockChange* RealLockChange = nullptr;
static TSetWeather* RealSetWeather = nullptr;
static TForceWeather* RealForceWeather = nullptr;

namespace Settings
{
int32_t* GetDifficulty() noexcept
{
    POINTER_SKYRIMSE(int32_t, s_difficulty, 381472);
    return s_difficulty.Get();
}

float* GetGreetDistance() noexcept
{
    POINTER_SKYRIMSE(float, s_greetDistance, 370892);
    return s_greetDistance.Get();
}

} // namespace Settings

namespace GameplayFormulas
{

float CalculateRealDamage(Actor* apHittee, float aDamage, bool aKillMove) noexcept
{
    using TGetDifficultyMultiplier = float(int32_t, int32_t, bool);
    POINTER_SKYRIMSE(TGetDifficultyMultiplier, s_getDifficultyMultiplier, 26503);

    bool isPlayer = apHittee == PlayerCharacter::Get();

    float multiplier = s_getDifficultyMultiplier(PlayerCharacter::Get()->difficulty, ActorValueInfo::kHealth, isPlayer);

    float realDamage = aDamage;

    // TODO(cosideci): this seems problematic? It may not register the kill for others?
    // Disabled for now, cause this check seems to have totally broken everything, let's see what happens.
    // if (!aKillMove || multiplier < 1.0)
    realDamage = aDamage * multiplier;

    return realDamage;
}

} // namespace GameplayFormulas

void FadeOutGame(bool aFadingOut, bool aBlackFade, float aFadeDuration, bool aRemainVisible, float aSecondsToFade) noexcept
{
    using TFadeOutGame = void(bool, bool, float, bool, float);
    POINTER_SKYRIMSE(TFadeOutGame, fadeOutGame, 52847);
    fadeOutGame.Get()(aFadingOut, aBlackFade, aFadeDuration, aRemainVisible, aSecondsToFade);
}

TESObjectREFR* TESObjectREFR::GetByHandle(uint32_t aHandle) noexcept
{
    TESObjectREFR* pResult = nullptr;

    using TGetRefrByHandle = void(uint32_t & aHandle, TESObjectREFR * &apResult);

    POINTER_SKYRIMSE(TGetRefrByHandle, s_getRefrByHandle, 17201);

    s_getRefrByHandle.Get()(aHandle, pResult);

    if (pResult)
        pResult->handleRefObject.DecRefHandle();

    return pResult;
}

BSPointerHandle<TESObjectREFR> TESObjectREFR::GetHandle() const noexcept
{
    TP_THIS_FUNCTION(TGetHandle, BSPointerHandle<TESObjectREFR>, const TESObjectREFR, BSPointerHandle<TESObjectREFR>* apResult);
    POINTER_SKYRIMSE(TGetHandle, s_getHandle, 19846);

    BSPointerHandle<TESObjectREFR> result{};
    TiltedPhoques::ThisCall(s_getHandle, this, &result);

    return result;
}

uint32_t* TESObjectREFR::GetNullHandle() noexcept
{
    POINTER_SKYRIMSE(uint32_t, s_nullHandle, 400312);

    return s_nullHandle.Get();
}

void TESObjectREFR::SetRotation(float aX, float aY, float aZ) noexcept
{
    TiltedPhoques::ThisCall(RealRotateX, this, aX);
    TiltedPhoques::ThisCall(RealRotateY, this, aY);
    TiltedPhoques::ThisCall(RealRotateZ, this, aZ);
}

using TiltedPhoques::Serialization;

void TESObjectREFR::SaveAnimationVariables(AnimationVariables& aVariables) const noexcept
{
    BSAnimationGraphManager* pManager = nullptr;
    if (animationGraphHolder.GetBSAnimationGraph(&pManager))
    {
        BSScopedLock<BSRecursiveLock> _{pManager->lock};

        if (pManager->animationGraphIndex < pManager->animationGraphs.size)
        {
            auto* pActor = Cast<Actor>(this);
            if (!pActor)
                return;

            const BShkbAnimationGraph* pGraph = nullptr;

            if (pActor->formID == 0x14)
                pGraph = pManager->animationGraphs.Get(0);
            else
                pGraph = pManager->animationGraphs.Get(pManager->animationGraphIndex);

            if (!pGraph)
                return;

            if (!pGraph->behaviorGraph || !pGraph->behaviorGraph->stateMachine || !pGraph->behaviorGraph->stateMachine->name)
                return;

            auto* pExtendedActor = pActor->GetExtension();
            if (pExtendedActor->GraphDescriptorHash == 0)
            {
                // Force third person graph to be used on player
                if (pActor->formID == 0x14)
                    pExtendedActor->GraphDescriptorHash = pManager->GetDescriptorKey(0);
                else
                    pExtendedActor->GraphDescriptorHash = pManager->GetDescriptorKey();
            }

            auto pDescriptor = AnimationGraphDescriptorManager::Get().GetDescriptor(pExtendedActor->GraphDescriptorHash);

            // Modded behavior check if descriptor wasn't found
            extern const AnimationGraphDescriptor* BehaviorVarPatch(BSAnimationGraphManager * pManager, Actor * pActor);
            if (!pDescriptor)
                pDescriptor = BehaviorVarPatch(pManager, pActor);

            if (!pDescriptor)
                return;

            const auto* pVariableSet = pGraph->behaviorGraph->animationVariables;

            if (!pVariableSet)
                return;

            aVariables.Booleans.assign(pDescriptor->BooleanLookUpTable.size(), false);
            aVariables.Floats.assign(pDescriptor->FloatLookupTable.size(), 0.f);
            aVariables.Integers.assign(pDescriptor->IntegerLookupTable.size(), 0);

            for (size_t i = 0; i < pDescriptor->BooleanLookUpTable.size(); ++i)
            {
                const auto idx = pDescriptor->BooleanLookUpTable[i];

                if (pVariableSet->data[idx] != 0)
                    aVariables.Booleans[i] = true;
            }

            for (size_t i = 0; i < pDescriptor->FloatLookupTable.size(); ++i)
            {
                const auto idx = pDescriptor->FloatLookupTable[i];

                aVariables.Floats[i] = *reinterpret_cast<float*>(&pVariableSet->data[idx]);
            }

            for (size_t i = 0; i < pDescriptor->IntegerLookupTable.size(); ++i)
            {
                const auto idx = pDescriptor->IntegerLookupTable[i];

                aVariables.Integers[i] = *reinterpret_cast<uint32_t*>(&pVariableSet->data[idx]);
            }
        }

        pManager->Release();
    }
}

void TESObjectREFR::LoadAnimationVariables(const AnimationVariables& aVariables) const noexcept
{
    BSAnimationGraphManager* pManager = nullptr;
    if (animationGraphHolder.GetBSAnimationGraph(&pManager))
    {
        BSScopedLock<BSRecursiveLock> _{pManager->lock};

        if (pManager->animationGraphIndex < pManager->animationGraphs.size)
        {
            const auto* pGraph = pManager->animationGraphs.Get(pManager->animationGraphIndex);

            if (!pGraph)
                return;

            if (!pGraph->behaviorGraph || !pGraph->behaviorGraph->stateMachine || !pGraph->behaviorGraph->stateMachine->name)
                return;

            auto* pActor = Cast<Actor>(this);
            if (!pActor)
                return;

            auto* pExtendedActor = pActor->GetExtension();
            if (pExtendedActor->GraphDescriptorHash == 0)
                pExtendedActor->GraphDescriptorHash = pManager->GetDescriptorKey();

            auto pDescriptor = AnimationGraphDescriptorManager::Get().GetDescriptor(pExtendedActor->GraphDescriptorHash);

            // Modded behavior check if descriptor wasn't found
            if (!pDescriptor)
                pDescriptor = BehaviorVarPatch(pManager, pActor);

            if (!pDescriptor)
                return;

            const auto* pVariableSet = pGraph->behaviorGraph->animationVariables;

            if (!pVariableSet)
                return;

            for (size_t i = 0; i < pDescriptor->BooleanLookUpTable.size(); ++i)
            {
                const auto idx = pDescriptor->BooleanLookUpTable[i];

#if TP_FALLOUT4
                if (pExtendedActor->IsRemotePlayer())
                {
                    if (!AnimationGraphDescriptor_Master_Behavior::TranslateThirdToFirstPerson(idx))
                        continue;
                }
#endif

                if (pVariableSet->size > idx)
                {
                    pVariableSet->data[idx] = aVariables.Booleans.size() > i ? aVariables.Booleans[i] : false;
                }
            }

            for (size_t i = 0; i < pDescriptor->FloatLookupTable.size(); ++i)
            {
                const auto idx = pDescriptor->FloatLookupTable[i];

#if TP_FALLOUT4
                if (pExtendedActor->IsRemotePlayer())
                {
                    if (!AnimationGraphDescriptor_Master_Behavior::TranslateThirdToFirstPerson(idx))
                        continue;
                }
#endif

                *reinterpret_cast<float*>(&pVariableSet->data[idx]) = aVariables.Floats.size() > i ? aVariables.Floats[i] : 0.f;
            }

            for (size_t i = 0; i < pDescriptor->IntegerLookupTable.size(); ++i)
            {
                const auto idx = pDescriptor->IntegerLookupTable[i];

#if TP_FALLOUT4
                if (pExtendedActor->IsRemotePlayer())
                {
                    if (!AnimationGraphDescriptor_Master_Behavior::TranslateThirdToFirstPerson(idx))
                        continue;
                }
#endif

                *reinterpret_cast<uint32_t*>(&pVariableSet->data[idx]) = aVariables.Integers.size() > i ? aVariables.Integers[i] : 0;
            }
        }

        pManager->Release();
    }
}

uint32_t TESObjectREFR::GetCellId() const noexcept
{
    if (!parentCell)
        return 0;

    const auto* pWorldSpace = parentCell->worldspace;

    return pWorldSpace != nullptr ? pWorldSpace->formID : parentCell->formID;
}

TESWorldSpace* TESObjectREFR::GetWorldSpace() const noexcept
{
    auto* pParentCell = parentCell ? parentCell : GetParentCell();
    if (pParentCell && !(pParentCell->cellFlags[0] & 1))
        return pParentCell->worldspace;

    return nullptr;
}

ExtraDataList* TESObjectREFR::GetExtraDataList() noexcept
{
#if TP_FALLOUT4
    return extraData;
#else
    return &extraData;
#endif
}

// Delete() should only be used on temporaries
void TESObjectREFR::Delete() const noexcept
{
    using ObjectReference = TESObjectREFR;

    PAPYRUS_FUNCTION(void, ObjectReference, Delete);

    s_pDelete(this);
}

void TESObjectREFR::Disable() const noexcept
{
    using ObjectReference = TESObjectREFR;

    PAPYRUS_FUNCTION(void, ObjectReference, Disable, bool);

    s_pDisable(this, true);
}

void TESObjectREFR::Enable() const noexcept
{
    using ObjectReference = TESObjectREFR;

    PAPYRUS_FUNCTION(void, ObjectReference, Enable, bool);

    s_pEnable(this, true);
}

// Skyrim: MoveTo() can fail, causing the object to be deleted
void TESObjectREFR::MoveTo(TESObjectCELL* apCell, const NiPoint3& acPosition) const noexcept
{
    ScopedReferencesOverride recursionGuard;

    TP_THIS_FUNCTION(TInternalMoveTo, bool, const TESObjectREFR, uint32_t*&, TESObjectCELL*, TESWorldSpace*, const NiPoint3&, const NiPoint3&);

    POINTER_SKYRIMSE(TInternalMoveTo, s_internalMoveTo, 56626);

    TiltedPhoques::ThisCall(s_internalMoveTo, this, GetNullHandle(), apCell, apCell->worldspace, acPosition, rotation);
}

void TESObjectREFR::PayGold(int32_t aAmount) noexcept
{
    ScopedInventoryOverride _;
    PayGoldToContainer(nullptr, aAmount);
}

void TESObjectREFR::PayGoldToContainer(TESObjectREFR* pContainer, int32_t aAmount) noexcept
{
    TP_THIS_FUNCTION(TPayGoldToContainer, void, TESObjectREFR, TESObjectREFR*, int32_t);
    POINTER_SKYRIMSE(TPayGoldToContainer, s_payGoldToContainer, 37511);
    TiltedPhoques::ThisCall(s_payGoldToContainer, this, pContainer, aAmount);
}

float Actor::GetSpeed() noexcept
{
    static BSFixedString speedSampledStr("SpeedSampled");
    float speed = 0.f;
    animationGraphHolder.GetVariableFloat(&speedSampledStr, &speed);

    return speed;
}

void Actor::SetSpeed(float aSpeed) noexcept
{
    static BSFixedString speedSampledStr("SpeedSampled");
    animationGraphHolder.SetVariableFloat(&speedSampledStr, aSpeed);
}

uint16_t Actor::GetLevel() const noexcept
{
    TP_THIS_FUNCTION(TGetLevel, uint16_t, const Actor);
    POINTER_SKYRIMSE(TGetLevel, s_getLevel, 37334);
    return TiltedPhoques::ThisCall(s_getLevel, this);
}

void Actor::ForcePosition(const NiPoint3& acPosition) noexcept
{
    ScopedReferencesOverride recursionGuard;

    // It just works TM
    SetPosition(acPosition, true);
}

void Actor::QueueUpdate() noexcept
{
    auto* pSetting = INISettingCollection::Get()->GetSetting("bUseFaceGenPreprocessedHeads:General");
    const auto originalValue = pSetting->data;
    pSetting->data = 0;

    TP_THIS_FUNCTION(TQueueUpdate, void, Actor, bool);
    POINTER_SKYRIMSE(TQueueUpdate, QueueUpdate, 40255);

    TiltedPhoques::ThisCall(QueueUpdate, this, true);

    pSetting->data = originalValue;
}

TESObjectCELL* TESWorldSpace::LoadCell(int32_t aXCoordinate, int32_t aYCoordinate) noexcept
{
    TP_THIS_FUNCTION(TLoadCell, TESObjectCELL*, TESWorldSpace, int32_t aXCoordinate, int32_t aYCoordinate);
    POINTER_SKYRIMSE(TLoadCell, s_loadCell, 20460);
    return TiltedPhoques::ThisCall(s_loadCell, this, aXCoordinate, aYCoordinate);
}

GamePtr<Actor> Actor::Create(TESNPC* apBaseForm) noexcept
{
    auto pActor = New();
    // Prevent saving
    pActor->SetSkipSaveFlag(true);
    pActor->GetExtension()->SetRemote(true);

    const auto pPlayer = static_cast<Actor*>(GetById(0x14));
    auto pCell = pPlayer->parentCell;
    const auto pWorldSpace = pPlayer->GetWorldSpace();

    pActor->SetLevelMod(4);
    pActor->MarkChanged(0x40000000);
    pActor->SetParentCell(pCell);
    pActor->SetBaseForm(apBaseForm);

    auto position = pPlayer->position;
    auto rotation = pPlayer->rotation;

    if (pCell && !(pCell->cellFlags[0] & 1))
        pCell = nullptr;

    ModManager::Get()->Spawn(position, rotation, pCell, pWorldSpace, pActor);

    pActor->ForcePosition(position);

    pActor->GetMagicCaster(MagicSystem::CastingSource::LEFT_HAND);
    pActor->GetMagicCaster(MagicSystem::CastingSource::RIGHT_HAND);
    pActor->GetMagicCaster(MagicSystem::CastingSource::OTHER);

    pActor->flags &= 0xFFDFFFFF;

    return pActor;
}

GamePtr<Actor> Actor::Spawn(uint32_t aBaseFormId) noexcept
{
    TESNPC* pNpc = Cast<TESNPC>(TESForm::GetById(aBaseFormId));
    return Actor::Create(pNpc);
}

void Actor::SetLevelMod(uint32_t aLevel) noexcept
{
    TP_THIS_FUNCTION(TActorSetLevelMod, void, ExtraDataList, uint32_t);
    POINTER_SKYRIMSE(TActorSetLevelMod, realSetLevelMod, 11806);

    const auto pExtraDataList = &extraData;

    TiltedPhoques::ThisCall(realSetLevelMod, pExtraDataList, aLevel);
}

ActorExtension* Actor::GetExtension() noexcept
{
    if (AsExActor())
    {
        return static_cast<ActorExtension*>(AsExActor());
    }

    if (AsExPlayerCharacter())
    {
        return static_cast<ActorExtension*>(AsExPlayerCharacter());
    }

    return nullptr;
}

ExActor* Actor::AsExActor() noexcept
{
    if (formType == Type && this != PlayerCharacter::Get())
        return static_cast<ExActor*>(this);

    return nullptr;
}

ExPlayerCharacter* Actor::AsExPlayerCharacter() noexcept
{
    if (this == PlayerCharacter::Get())
        return static_cast<ExPlayerCharacter*>(this);

    return nullptr;
}

PlayerCharacter* PlayerCharacter::Get() noexcept
{
    POINTER_SKYRIMSE(PlayerCharacter*, s_character, 401069);

    return *s_character.Get();
}

const GameArray<TintMask*>& PlayerCharacter::GetTints() const noexcept
{
    if (overlayTints)
        return *overlayTints;

    return baseTints;
}

Lock* TESObjectREFR::GetLock() const noexcept
{
    TP_THIS_FUNCTION(TGetLock, Lock*, const TESObjectREFR);
    POINTER_SKYRIMSE(TGetLock, realGetLock, 20223);

    return TiltedPhoques::ThisCall(realGetLock, this);
}

Lock* TESObjectREFR::CreateLock() noexcept
{
    TP_THIS_FUNCTION(TCreateLock, Lock*, TESObjectREFR);
    POINTER_SKYRIMSE(TCreateLock, realCreateLock, 20221);

    return TiltedPhoques::ThisCall(realCreateLock, this);
}

void TESObjectREFR::LockChange() noexcept
{
    TiltedPhoques::ThisCall(RealLockChange, this);
}

const float TESObjectREFR::GetHeight() noexcept
{
    auto boundMax = GetBoundMax();
    return boundMax.z - GetBoundMin().z;
}

TESObjectREFR::OpenState TESObjectREFR::GetOpenState() noexcept
{
    using ObjectReference = TESObjectREFR;

    PAPYRUS_FUNCTION(TESObjectREFR::OpenState, ObjectReference, GetOpenState);

    return s_pGetOpenState(this);
}

bool ActorState::SetWeaponDrawn(bool aDraw) noexcept
{
    TP_THIS_FUNCTION(TSetWeaponState, bool, ActorState, bool aDraw);
    POINTER_SKYRIMSE(TSetWeaponState, setWeaponState, 38979);

    return TiltedPhoques::ThisCall(setWeaponState, this, aDraw);
}

extern thread_local bool g_forceAnimation;

void Actor::SetWeaponDrawnEx(bool aDraw) noexcept
{
    spdlog::debug("Setting weapon drawn: {:X}:{}, current state: {}", formID, aDraw, actorState.IsWeaponDrawn());

    if (actorState.IsWeaponDrawn() == aDraw)
    {
        actorState.SetWeaponDrawn(!aDraw);

        spdlog::debug("Setting weapon drawn after update: {:X}:{}, current state: {}", formID, aDraw, actorState.IsWeaponDrawn());
    }

    g_forceAnimation = true;
    SetWeaponDrawn(aDraw);
    g_forceAnimation = false;
}

static thread_local bool s_execInitPackage = false;

void Actor::SetPackage(TESPackage* apPackage) noexcept
{
    s_execInitPackage = true;
    PutCreatedPackage(apPackage);
    s_execInitPackage = false;
}

void Actor::SetPlayerRespawnMode(bool aSet) noexcept
{
    SetEssentialEx(aSet);
    // Makes the player go in an unrecoverable bleedout state
    SetNoBleedoutRecovery(aSet);

    if (formID != 0x14)
    {
        auto pPlayerFaction = Cast<TESFaction>(TESForm::GetById(0xDB1));
        SetFactionRank(pPlayerFaction, 1);
    }
}

void Actor::SetEssentialEx(bool aSet) noexcept
{
    SetEssential(aSet);
    TESNPC* pBase = Cast<TESNPC>(baseForm);
    if (pBase)
        pBase->actorData.SetEssential(aSet);
}

void Actor::SetNoBleedoutRecovery(bool aSet) noexcept
{
    TP_THIS_FUNCTION(TSetNoBleedoutRecovery, void, Actor, bool);
    POINTER_SKYRIMSE(TSetNoBleedoutRecovery, s_setNoBleedoutRecovery, 38533);
    TiltedPhoques::ThisCall(s_setNoBleedoutRecovery, this, aSet);
}

void Actor::DispelAllSpells(bool aNow) noexcept
{
    magicTarget.DispelAllSpells(aNow);
}

void MagicTarget::DispelAllSpells(bool aNow) noexcept
{
    TP_THIS_FUNCTION(TDispelAllSpells, void, MagicTarget, bool aNow);
    POINTER_SKYRIMSE(TDispelAllSpells, dispelAllSpells, 34512);
    TiltedPhoques::ThisCall(dispelAllSpells, this, aNow);
}

void TESObjectCELL::GetCOCPlacementInfo(NiPoint3* aOutPos, NiPoint3* aOutRot, bool aAllowCellLoad) noexcept
{
    TP_THIS_FUNCTION(TGetCOCPlacementInfo, void, TESObjectCELL, NiPoint3*, NiPoint3*, bool);
    POINTER_SKYRIMSE(TGetCOCPlacementInfo, s_getCOCPlacementInfo, 19075);
    TiltedPhoques::ThisCall(s_getCOCPlacementInfo, this, aOutPos, aOutRot, aAllowCellLoad);
}

void PlayerCharacter::SetGodMode(bool aSet) noexcept
{
    POINTER_SKYRIMSE(bool, bGodMode, 404238);
    *bGodMode.Get() = aSet;
}

void AIProcess::KnockExplosion(Actor* apActor, const NiPoint3* aSourceLocation, float afMagnitude)
{
    TP_THIS_FUNCTION(TKnockExplosion, void, AIProcess, Actor*, const NiPoint3*, float);
    POINTER_SKYRIMSE(TKnockExplosion, knockExplosion, 39895);
    TiltedPhoques::ThisCall(knockExplosion, this, apActor, aSourceLocation, afMagnitude);
}

bool Actor::IsInCombat() const noexcept
{
    PAPYRUS_FUNCTION(bool, Actor, IsInCombat);
    return s_pIsInCombat(this);
}

Actor* Actor::GetCombatTarget() const noexcept
{
    PAPYRUS_FUNCTION(Actor*, Actor, GetCombatTarget);
    return s_pGetCombatTarget(this);
}

// TODO: this is a really hacky solution.
// The internal targeting system should be disabled instead.
void Actor::StartCombatEx(Actor* apTarget) noexcept
{
    if (GetCombatTarget() != apTarget)
    {
        StopCombat();
        StartCombat(apTarget);
    }
}

void Actor::SetCombatTargetEx(Actor* apTarget) noexcept
{
    if (pCombatController)
        pCombatController->SetTarget(apTarget);
}

void Actor::StartCombat(Actor* apTarget) noexcept
{
    PAPYRUS_FUNCTION(void, Actor, StartCombat, Actor*);
    s_pStartCombat(this, apTarget);
}

void Actor::StopCombat() noexcept
{
    PAPYRUS_FUNCTION(void, Actor, StopCombat);
    s_pStopCombat(this);
}

bool Actor::HasPerk(uint32_t aPerkFormId) const noexcept
{
    return GetPerkRank(aPerkFormId) != 0;
}

uint8_t Actor::GetPerkRank(uint32_t aPerkFormId) const noexcept
{
    BGSPerk* pPerk = Cast<BGSPerk>(TESForm::GetById(aPerkFormId));
    if (!pPerk)
        return 0;

    TP_THIS_FUNCTION(TGetPerkRank, uint8_t, const Actor, BGSPerk*);
    POINTER_SKYRIMSE(TGetPerkRank, getPerkRank, 37698);
    // TODO(ft)

    return TiltedPhoques::ThisCall(getPerkRank, this, pPerk);
}

Sky* Sky::Get() noexcept
{
    using SkyGet = Sky*(__fastcall)();

    POINTER_SKYRIMSE(SkyGet, skyGet, 13878);

    return skyGet.Get()();
}

void Sky::SetWeather(TESWeather* apWeather) noexcept
{
    // TODO: verify the use of these bools
    TiltedPhoques::ThisCall(RealSetWeather, this, apWeather, true, true);
}

void Sky::ForceWeather(TESWeather* apWeather) noexcept
{
    // TODO: verify the use of abOverride
    TiltedPhoques::ThisCall(RealForceWeather, this, apWeather, true);
}

void Sky::ReleaseWeatherOverride() noexcept
{
    TP_THIS_FUNCTION(TReleaseWeatherOverride, void, Sky);
    POINTER_SKYRIMSE(TReleaseWeatherOverride, releaseWeatherOverride, 26244);

    TiltedPhoques::ThisCall(releaseWeatherOverride, this);
}

void Sky::ResetWeather() noexcept
{
    TP_THIS_FUNCTION(TResetWeather, void, Sky);
    POINTER_SKYRIMSE(TResetWeather, resetWeather, 26242);

    TiltedPhoques::ThisCall(resetWeather, this);
}

TESWeather* Sky::GetWeather() const noexcept
{
    return pCurrentWeather;
}

char TP_MAKE_THISCALL(HookSetPosition, Actor, NiPoint3& aPosition)
{
    const auto pExtension = apThis ? apThis->GetExtension() : nullptr;
    const auto bIsRemote = pExtension && pExtension->IsRemote();

    if (bIsRemote && !ScopedReferencesOverride::IsOverriden())
        return 1;

    // Don't interfere with non actor references, or the player, or if we are calling our self
    if (apThis->formType != Actor::Type || apThis == PlayerCharacter::Get() || ScopedReferencesOverride::IsOverriden())
        return TiltedPhoques::ThisCall(RealSetPosition, apThis, aPosition);

    ScopedReferencesOverride recursionGuard;

    // It just works TM
    apThis->SetPosition(aPosition, false);

    return 1;
}

void TP_MAKE_THISCALL(HookRotateX, TESObjectREFR, float aAngle)
{
    if (apThis->formType == Actor::Type)
    {
        const auto pActor = static_cast<Actor*>(apThis);
        // We don't allow remotes to move
        if (pActor->GetExtension()->IsRemote())
            return;
    }

    return TiltedPhoques::ThisCall(RealRotateX, apThis, aAngle);
}

void TP_MAKE_THISCALL(HookRotateY, TESObjectREFR, float aAngle)
{
    if (apThis->formType == Actor::Type)
    {
        const auto pActor = static_cast<Actor*>(apThis);
        // We don't allow remotes to move
        if (pActor->GetExtension()->IsRemote())
            return;
    }

    return TiltedPhoques::ThisCall(RealRotateY, apThis, aAngle);
}

void TP_MAKE_THISCALL(HookRotateZ, TESObjectREFR, float aAngle)
{
    if (apThis->formType == Actor::Type)
    {
        const auto pActor = static_cast<Actor*>(apThis);
        // We don't allow remotes to move
        if (pActor->GetExtension()->IsRemote())
            return;
    }

    return TiltedPhoques::ThisCall(RealRotateZ, apThis, aAngle);
}

char TP_MAKE_THISCALL(HookActorProcess, Actor, float a2)
{
    // Don't process AI if we own the actor

    if (apThis->GetExtension()->IsRemote())
        return 0;

    return TiltedPhoques::ThisCall(RealActorProcess, apThis, a2);
}

void TP_MAKE_THISCALL(HookLockChange, TESObjectREFR)
{
    const auto* pLock = apThis->GetLock();
    uint8_t lockLevel = pLock->lockLevel;

    World::Get().GetRunner().Trigger(LockChangeEvent(apThis->formID, pLock->flags, lockLevel));

    TiltedPhoques::ThisCall(RealLockChange, apThis);
}

// Disable AI sync for now, experiment didn't work, code might be useful later on though.
#define AI_SYNC 0

TP_THIS_FUNCTION(TCheckForNewPackage, bool, void, Actor* apActor, uint64_t aUnk1);
static TCheckForNewPackage* RealCheckForNewPackage = nullptr;

bool TP_MAKE_THISCALL(HookCheckForNewPackage, void, Actor* apActor, uint64_t aUnk1)
{
#if AI_SYNC
    if (apActor && apActor->GetExtension()->IsRemote())
        return false;

#endif
    return TiltedPhoques::ThisCall(RealCheckForNewPackage, apThis, apActor, aUnk1);
}

TP_THIS_FUNCTION(TInitFromPackage, void, void, TESPackage* apPackage, TESObjectREFR* apTarget, Actor* arActor);
static TInitFromPackage* RealInitFromPackage = nullptr;

void TP_MAKE_THISCALL(HookInitFromPackage, void, TESPackage* apPackage, TESObjectREFR* apTarget, Actor* arActor)
{
#if AI_SYNC
    // This guard is here for when the client sets the package based on a remote message
    if (s_execInitPackage)
        return TiltedPhoques::ThisCall(RealInitFromPackage, apThis, apPackage, apTarget, arActor);

    if (arActor && arActor->GetExtension()->IsRemote())
        return;

    if (arActor && apPackage)
        World::Get().GetRunner().Trigger(InitPackageEvent(arActor->formID, apPackage->formID));

#endif
    return TiltedPhoques::ThisCall(RealInitFromPackage, apThis, apPackage, apTarget, arActor);
}

TP_THIS_FUNCTION(TSetCurrentPickREFR, void, Console, BSPointerHandle<TESObjectREFR>* apRefr);
static TSetCurrentPickREFR* RealSetCurrentPickREFR = nullptr;

void TP_MAKE_THISCALL(HookSetCurrentPickREFR, Console, BSPointerHandle<TESObjectREFR>* apRefr)
{
    uint32_t formId = 0;

    TESObjectREFR* pObject = TESObjectREFR::GetByHandle(apRefr->handle.iBits);
    if (pObject)
        formId = pObject->formID;

    World::Get().GetDebugService().SetDebugId(formId);

    return TiltedPhoques::ThisCall(RealSetCurrentPickREFR, apThis, apRefr);
}

void TP_MAKE_THISCALL(HookSetWeather, Sky, TESWeather* apWeather, bool abOverride, bool abAccelerate)
{
#if 0
    spdlog::debug("Set weather form id: {:X}, override: {}, accelerate: {}", apWeather ? apWeather->formID : 0, abOverride, abAccelerate);

    if (!Sky::s_shouldUpdateWeather)
        return;
#endif

    TiltedPhoques::ThisCall(RealSetWeather, apThis, apWeather, abOverride, abAccelerate);
}

void TP_MAKE_THISCALL(HookForceWeather, Sky, TESWeather* apWeather, bool abOverride)
{
#if 0
    spdlog::debug("Force weather form id: {:X}, override: {}", apWeather ? apWeather->formID : 0, abOverride);

    if (!Sky::s_shouldUpdateWeather)
        return;
#endif

    TiltedPhoques::ThisCall(RealForceWeather, apThis, apWeather, abOverride);
}

TP_THIS_FUNCTION(TUpdateWeather, void, Sky);
static TUpdateWeather* RealUpdateWeather = nullptr;

void TP_MAKE_THISCALL(HookUpdateWeather, Sky)
{
#if 0
    if (!Sky::s_shouldUpdateWeather)
        return;
#endif

    TiltedPhoques::ThisCall(RealUpdateWeather, apThis);
}

TP_THIS_FUNCTION(TAddDeathItems, void, Actor);
static TAddDeathItems* RealAddDeathItems = nullptr;

void TP_MAKE_THISCALL(HookAddDeathItems, Actor)
{
    if (apThis->GetExtension()->IsRemote())
        return;

    TiltedPhoques::ThisCall(RealAddDeathItems, apThis);
}

TP_THIS_FUNCTION(TIsFleeing, bool, Actor);
static TIsFleeing* RealIsFleeing = nullptr;

bool TP_MAKE_THISCALL(HookIsFleeing, Actor)
{
    // TODO: Player or RemotePlayer? Can players be in fleeing mode in skyrim?
    // TODO: investigate why the flee flag is set at all on remote players sometimes.
    if (apThis->GetExtension()->IsPlayer())
        return false;
    
    return TiltedPhoques::ThisCall(RealIsFleeing, apThis);
}

TiltedPhoques::Initializer s_referencesHooks(
    []()
    {
        POINTER_SKYRIMSE(TSetPosition, s_setPosition, 19790);
        POINTER_SKYRIMSE(TRotate, s_rotateX, 19787);
        POINTER_SKYRIMSE(TRotate, s_rotateY, 19788);
        POINTER_SKYRIMSE(TRotate, s_rotateZ, 19789);
        POINTER_SKYRIMSE(TActorProcess, s_actorProcess, 37356);
        POINTER_SKYRIMSE(TLockChange, s_lockChange, 19512);
        POINTER_SKYRIMSE(TCheckForNewPackage, s_checkForNewPackage, 39114);
        POINTER_SKYRIMSE(TInitFromPackage, s_initFromPackage, 38959);
        POINTER_SKYRIMSE(TSetCurrentPickREFR, s_setCurrentPickREFR, 51093);
        POINTER_SKYRIMSE(TSetWeather, setWeather, 26241);
        POINTER_SKYRIMSE(TForceWeather, forceWeather, 26243);
        POINTER_SKYRIMSE(TUpdateWeather, updateWeather, 26231);
        POINTER_SKYRIMSE(TAddDeathItems, addDeathItems, 37198);
        POINTER_SKYRIMSE(TIsFleeing, isFleeing, 37577);

        RealSetPosition = s_setPosition.Get();
        RealRotateX = s_rotateX.Get();
        RealRotateY = s_rotateY.Get();
        RealRotateZ = s_rotateZ.Get();
        RealActorProcess = s_actorProcess.Get();
        RealLockChange = s_lockChange.Get();
        RealCheckForNewPackage = s_checkForNewPackage.Get();
        RealInitFromPackage = s_initFromPackage.Get();
        RealSetCurrentPickREFR = s_setCurrentPickREFR.Get();
        RealSetWeather = setWeather.Get();
        RealForceWeather = forceWeather.Get();
        RealUpdateWeather = updateWeather.Get();
        RealAddDeathItems = addDeathItems.Get();
        RealIsFleeing = isFleeing.Get();

        TP_HOOK(&RealSetPosition, HookSetPosition);
        TP_HOOK(&RealRotateX, HookRotateX);
        TP_HOOK(&RealRotateY, HookRotateY);
        TP_HOOK(&RealRotateZ, HookRotateZ);
        TP_HOOK(&RealActorProcess, HookActorProcess);
        TP_HOOK(&RealLockChange, HookLockChange);
        TP_HOOK(&RealCheckForNewPackage, HookCheckForNewPackage);
        TP_HOOK(&RealInitFromPackage, HookInitFromPackage);
        TP_HOOK(&RealSetCurrentPickREFR, HookSetCurrentPickREFR);
        TP_HOOK(&RealSetWeather, HookSetWeather);
        TP_HOOK(&RealForceWeather, HookForceWeather);
        TP_HOOK(&RealUpdateWeather, HookUpdateWeather);
        TP_HOOK(&RealAddDeathItems, HookAddDeathItems);
        TP_HOOK(&RealIsFleeing, HookIsFleeing);
    });
