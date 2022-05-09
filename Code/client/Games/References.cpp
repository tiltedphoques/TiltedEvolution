#include <TiltedOnlinePCH.h>

#include <Games/References.h>

#include <RTTI.h>

#include <Forms/TESWorldSpace.h>
#include <Forms/TESObjectCELL.h>
#include <Forms/BGSHeadPart.h>
#include <Forms/TESNPC.h>
#include <Forms/TESPackage.h>
#include <SaveLoad.h>
#include <Games/ExtraDataList.h>

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
#include <Magic/MagicCaster.h>

#include <Events/LockChangeEvent.h>
#include <Events/InitPackageEvent.h>
#include <Events/DialogueEvent.h>

#include <TiltedCore/Serialization.hpp>

#include <Services/PapyrusService.h>
#include <World.h>

using ScopedReferencesOverride = ScopedOverride<TESObjectREFR>;
thread_local uint32_t ScopedReferencesOverride::s_refCount = 0;

TP_THIS_FUNCTION(TSetPosition, char, Actor, NiPoint3& acPosition);
TP_THIS_FUNCTION(TRotate, void, TESObjectREFR, float aAngle);
TP_THIS_FUNCTION(TActorProcess, char, Actor, float aValue);
TP_THIS_FUNCTION(TLockChange, void, TESObjectREFR);

static TSetPosition* RealSetPosition = nullptr;
static TRotate* RealRotateX = nullptr;
static TRotate* RealRotateY = nullptr;
static TRotate* RealRotateZ = nullptr;
static TActorProcess* RealActorProcess = nullptr;
static TLockChange* RealLockChange = nullptr;

namespace GameplayFormulas
{

float CalculateRealDamage(Actor* apHittee, float aDamage) noexcept
{
    using TGetDifficultyMultiplier = float(int32_t, int32_t, bool);
    POINTER_SKYRIMSE(TGetDifficultyMultiplier, s_getDifficultyMultiplier, 26503);

    bool isPlayer = apHittee == PlayerCharacter::Get();
    float multiplier = s_getDifficultyMultiplier(PlayerCharacter::Get()->difficulty, ActorValueInfo::kHealth, isPlayer);

    float realDamage = aDamage;

    if (fabs(aDamage) <= 0.000099999997 || multiplier < 1.0)
        realDamage = aDamage * multiplier;

    return realDamage;
}

}

TESObjectREFR* TESObjectREFR::GetByHandle(uint32_t aHandle) noexcept
{
    TESObjectREFR* pResult = nullptr;

    using TGetRefrByHandle = void(uint32_t& aHandle, TESObjectREFR*& apResult);

    POINTER_SKYRIMSE(TGetRefrByHandle, s_getRefrByHandle, 17201);
    POINTER_FALLOUT4(TGetRefrByHandle, s_getRefrByHandle, 0x140023740 - 0x140000000);

    s_getRefrByHandle.Get()(aHandle, pResult);

    if (pResult)
        pResult->handleRefObject.DecRefHandle();

    return pResult;
}

uint32_t* TESObjectREFR::GetNullHandle() noexcept
{
    POINTER_SKYRIMSE(uint32_t, s_nullHandle, 400312);
    POINTER_FALLOUT4(uint32_t, s_nullHandle, 0x1438CCE04 - 0x140000000);

    return s_nullHandle.Get();
}

void TESObjectREFR::SetRotation(float aX, float aY, float aZ) noexcept
{
    ThisCall(RealRotateX, this, aX);
    ThisCall(RealRotateY, this, aY);
    ThisCall(RealRotateZ, this, aZ);
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
        
            if (!pGraph->behaviorGraph || !pGraph->behaviorGraph->stateMachine ||
                !pGraph->behaviorGraph->stateMachine->name)
                return;

            auto* pExtendedActor = pActor->GetExtension();
            if (pExtendedActor->GraphDescriptorHash == 0)
            {
                // Force first person graph to be used on player
                if (pActor->formID == 0x14)
                    pExtendedActor->GraphDescriptorHash = pManager->GetDescriptorKey(0);
                else
                    pExtendedActor->GraphDescriptorHash = pManager->GetDescriptorKey();
            }

            auto pDescriptor =
                AnimationGraphDescriptorManager::Get().GetDescriptor(pExtendedActor->GraphDescriptorHash);

            if (!pDescriptor)
                return;

            const auto* pVariableSet = pGraph->behaviorGraph->animationVariables;

            if (!pVariableSet)
                return;
            
            aVariables.Booleans = 0;

            aVariables.Floats.resize(pDescriptor->FloatLookupTable.size());
            aVariables.Integers.resize(pDescriptor->IntegerLookupTable.size());

            for (size_t i = 0; i < pDescriptor->BooleanLookUpTable.size(); ++i)
            {
                const auto idx = pDescriptor->BooleanLookUpTable[i];

                if (pVariableSet->data[idx] != 0)
                    aVariables.Booleans |= (1ull << i);
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
            
            if (!pGraph->behaviorGraph || !pGraph->behaviorGraph->stateMachine ||
                !pGraph->behaviorGraph->stateMachine->name)
                return;

            auto* pActor = Cast<Actor>(this);
            if (!pActor)
                return;

            auto* pExtendedActor = pActor->GetExtension();
            if (pExtendedActor->GraphDescriptorHash == 0)
                pExtendedActor->GraphDescriptorHash = pManager->GetDescriptorKey();

            auto pDescriptor =
                AnimationGraphDescriptorManager::Get().GetDescriptor(pExtendedActor->GraphDescriptorHash);

            if (!pDescriptor)
            {
                //if ((formID & 0xFF000000) == 0xFF000000)
                    //spdlog::info("Form id {} has {}", formID, pGraph->behaviorGraph->stateMachine->name);
                return;
            }

            const auto* pVariableSet = pGraph->behaviorGraph->animationVariables;
            
            if (!pVariableSet)
                return;
            
            for (size_t i = 0; i < pDescriptor->BooleanLookUpTable.size(); ++i)
            {
                const auto idx = pDescriptor->BooleanLookUpTable[i];

                if (pVariableSet->size > idx)
                {
                    pVariableSet->data[idx] = (aVariables.Booleans & (1ull << i)) != 0;
                }
            }

            for (size_t i = 0; i < pDescriptor->FloatLookupTable.size(); ++i)
            {
                const auto idx = pDescriptor->FloatLookupTable[i];
                *reinterpret_cast<float*>(&pVariableSet->data[idx]) = aVariables.Floats.size() > i ? aVariables.Floats[i] : 0.f;
            }

            for (size_t i = 0; i < pDescriptor->IntegerLookupTable.size(); ++i)
            {
                const auto idx = pDescriptor->IntegerLookupTable[i];
                *reinterpret_cast<uint32_t*>(&pVariableSet->data[idx]) = aVariables.Integers.size() > i ? aVariables.Integers[i] : 0;
            }
        }

        pManager->Release();
    }
}

String TESObjectREFR::SerializeInventory() const noexcept
{
    ScopedSaveLoadOverride _;

    // TODO: buffer[1 << 15] is too small for some inventories
    // buffer[1 << 18] does the job, but these inventories seem to be bugged
    // ask cosi for repro
    // temp solution: increase the buffer
    // only happened in skyrim, idk if fallout 4 needs it
    char buffer[1 << 18];
    BGSSaveFormBuffer saveBuffer;
    saveBuffer.buffer = buffer;
    saveBuffer.capacity = 1 << 18;
    saveBuffer.changeFlags = 1024;

    SaveInventory(&saveBuffer);

    return String(buffer, saveBuffer.position);
}

void TESObjectREFR::DeserializeInventory(const String& acData) noexcept
{
    ScopedSaveLoadOverride _;

    BGSLoadFormBuffer loadBuffer(1024);
    loadBuffer.SetSize(acData.size() & 0xFFFFFFFF);
    loadBuffer.buffer = acData.c_str();
    loadBuffer.formId = 0;
    loadBuffer.form = nullptr;
    
    LoadInventory(&loadBuffer);
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

    TP_THIS_FUNCTION(TInternalMoveTo, bool, const TESObjectREFR, uint32_t*&, TESObjectCELL*, TESWorldSpace*,
                     const NiPoint3&, const NiPoint3&);

    POINTER_SKYRIMSE(TInternalMoveTo, s_internalMoveTo, 56626);
    POINTER_FALLOUT4(TInternalMoveTo, s_internalMoveTo, 0x1413FE7E0 - 0x140000000);

    ThisCall(s_internalMoveTo, this, GetNullHandle(), apCell, apCell->worldspace, acPosition, rotation);
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
    ThisCall(s_payGoldToContainer, this, pContainer, aAmount);
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

#ifdef TP_SKYRIM
    TP_THIS_FUNCTION(TQueueUpdate, void, Actor, bool);
#else
    TP_THIS_FUNCTION(TQueueUpdate, void, Actor, bool aFaceGen, uint32_t, bool, uint32_t);
#endif

    POINTER_SKYRIMSE(TQueueUpdate, QueueUpdate, 40255);
    POINTER_FALLOUT4(TQueueUpdate, QueueUpdate, 0x140D8A1F0 - 0x140000000);

#ifdef TP_SKYRIM
    ThisCall(QueueUpdate, this, true);
#else
    ThisCall(QueueUpdate, this, true, 0, true, 0);
#endif

    pSetting->data = originalValue;
}

TESObjectCELL* TESWorldSpace::LoadCell(int32_t aXCoordinate, int32_t aYCoordinate) noexcept
{
    TP_THIS_FUNCTION(TLoadCell, TESObjectCELL*, TESWorldSpace, int32_t aXCoordinate, int32_t aYCoordinate);
    POINTER_SKYRIMSE(TLoadCell, s_loadCell, 20460);
    return ThisCall(s_loadCell, this, aXCoordinate, aYCoordinate);
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

#if TP_SKYRIM
    pActor->GetMagicCaster(MagicSystem::CastingSource::LEFT_HAND);
    pActor->GetMagicCaster(MagicSystem::CastingSource::RIGHT_HAND);
    pActor->GetMagicCaster(MagicSystem::CastingSource::OTHER);
#endif

    pActor->flags &= 0xFFDFFFFF;

    return pActor;
}


void Actor::SetLevelMod(uint32_t aLevel) noexcept
{
    TP_THIS_FUNCTION(TActorSetLevelMod, void, ExtraDataList, uint32_t);
    POINTER_SKYRIMSE(TActorSetLevelMod, realSetLevelMod, 11806);
    POINTER_FALLOUT4(TActorSetLevelMod, realSetLevelMod, 0x14008F660 - 0x140000000);

#if TP_FALLOUT4
    const auto pExtraDataList = extraData;
#else
    const auto pExtraDataList = &extraData;
#endif

    ThisCall(realSetLevelMod, pExtraDataList, aLevel);
}

ActorExtension* Actor::GetExtension() noexcept
{
    if(AsExActor())
    {
        return static_cast<ActorExtension*>(AsExActor());
    }

    if(AsExPlayerCharacter())
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
    POINTER_FALLOUT4(PlayerCharacter*, s_character, 0x145AA4388 - 0x140000000);
    POINTER_SKYRIMSE(PlayerCharacter*, s_character, 401069);

    return *s_character.Get();
}

#if TP_SKYRIM
const GameArray<TintMask*>& PlayerCharacter::GetTints() const noexcept
{
    if (overlayTints)
        return *overlayTints;

    return baseTints;
}
#endif

Lock* TESObjectREFR::GetLock() noexcept
{
    TP_THIS_FUNCTION(TGetLock, Lock*, TESObjectREFR);
    POINTER_SKYRIMSE(TGetLock, realGetLock, 20223);
    POINTER_FALLOUT4(TGetLock, realGetLock, 0x14047FEE0 - 0x140000000);

    return ThisCall(realGetLock, this);
}

Lock* TESObjectREFR::CreateLock() noexcept
{
    TP_THIS_FUNCTION(TCreateLock, Lock*, TESObjectREFR);
    POINTER_SKYRIMSE(TCreateLock, realCreateLock, 20221);
    POINTER_FALLOUT4(TCreateLock, realCreateLock, 0x14047FD20 - 0x140000000);

    return ThisCall(realCreateLock, this);
}

void TESObjectREFR::LockChange() noexcept
{
    ThisCall(RealLockChange, this);
}

const float TESObjectREFR::GetHeight() noexcept
{
    auto boundMax = GetBoundMax();
    return boundMax.z - GetBoundMin().z;
}

bool ActorState::SetWeaponDrawn(bool aDraw) noexcept
{
    TP_THIS_FUNCTION(TSetWeaponState, bool, ActorState, bool aDraw);

    POINTER_SKYRIMSE(TSetWeaponState, setWeaponState, 38979);
    POINTER_FALLOUT4(TSetWeaponState, setWeaponState, 0x140E22DF0 - 0x140000000);

    return ThisCall(setWeaponState, this, aDraw);
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

char TP_MAKE_THISCALL(HookSetPosition, Actor, NiPoint3& aPosition)
{
    const auto pExtension = apThis ? apThis->GetExtension() : nullptr;
    const auto bIsRemote = pExtension && pExtension->IsRemote();

    if (bIsRemote && !ScopedReferencesOverride::IsOverriden())
        return 1;

    // Don't interfere with non actor references, or the player, or if we are calling our self
    if (apThis->formType != Actor::Type || apThis == PlayerCharacter::Get() || ScopedReferencesOverride::IsOverriden())
        return ThisCall(RealSetPosition, apThis, aPosition);

    ScopedReferencesOverride recursionGuard;

    // It just works TM
    apThis->SetPosition(aPosition, false);

    return 1;
}

void TP_MAKE_THISCALL(HookRotateX, TESObjectREFR, float aAngle)
{
    if(apThis->formType == Actor::Type)
    {
        const auto pActor = static_cast<Actor*>(apThis);
        // We don't allow remotes to move
        if (pActor->GetExtension()->IsRemote())
            return;
    }

    return ThisCall(RealRotateX, apThis, aAngle);
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

    return ThisCall(RealRotateY, apThis, aAngle);
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

    return ThisCall(RealRotateZ, apThis, aAngle);
}

char TP_MAKE_THISCALL(HookActorProcess, Actor, float a2)
{
    // Don't process AI if we own the actor

    if (apThis->GetExtension()->IsRemote())
        return 0;

    return ThisCall(RealActorProcess, apThis, a2);
}

void TP_MAKE_THISCALL(HookLockChange, TESObjectREFR)
{
    const auto* pLock = apThis->GetLock();
    uint8_t lockLevel = pLock->lockLevel;

    World::Get().GetRunner().Trigger(LockChangeEvent(apThis->formID, pLock->flags, lockLevel));

    ThisCall(RealLockChange, apThis);
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
    return ThisCall(RealCheckForNewPackage, apThis, apActor, aUnk1);
}

TP_THIS_FUNCTION(TInitFromPackage, void, void, TESPackage* apPackage, TESObjectREFR* apTarget, Actor* arActor);
static TInitFromPackage* RealInitFromPackage = nullptr;

void TP_MAKE_THISCALL(HookInitFromPackage, void, TESPackage* apPackage, TESObjectREFR* apTarget, Actor* arActor)
{
#if AI_SYNC
    // This guard is here for when the client sets the package based on a remote message
    if (s_execInitPackage)
        return ThisCall(RealInitFromPackage, apThis, apPackage, apTarget, arActor);

    if (arActor && arActor->GetExtension()->IsRemote())
        return;

    if (arActor && apPackage)
        World::Get().GetRunner().Trigger(InitPackageEvent(arActor->formID, apPackage->formID));

#endif
    return ThisCall(RealInitFromPackage, apThis, apPackage, apTarget, arActor);
}

TP_THIS_FUNCTION(TSpeakSoundFunction, bool, Actor, const char* apName, uint32_t* a3, uint32_t a4, uint32_t a5, uint32_t a6, uint64_t a7, uint64_t a8, uint64_t a9, bool a10, uint64_t a11, bool a12, bool a13, bool a14);
static TSpeakSoundFunction* RealSpeakSoundFunction = nullptr;

bool TP_MAKE_THISCALL(HookSpeakSoundFunction, Actor, const char* apName, uint32_t* a3, uint32_t a4, uint32_t a5, uint32_t a6, uint64_t a7, uint64_t a8, uint64_t a9, bool a10, uint64_t a11, bool a12, bool a13, bool a14)
{
    spdlog::debug("a3: {:X}, a4: {}, a5: {}, a6: {}, a7: {}, a8: {:X}, a9: {:X}, a10: {}, a11: {:X}, a12: {}, a13: {}, a14: {}",
                  (uint64_t)a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14);

    /*
    if (apThis->GetExtension()->IsLocal())
        World::Get().GetRunner().Trigger(DialogueEvent(apThis->formID, apName));
    */

    return ThisCall(RealSpeakSoundFunction, apThis, apName, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14);
}

void Actor::SpeakSound(const char* pFile)
{
    uint32_t handle[3]{};
    handle[0] = -1;
    ThisCall(RealSpeakSoundFunction, this, pFile, handle, 0, 0x32, 0, 0, 0, 0, 0, 0, 0, 1, 1);
}

TiltedPhoques::Initializer s_referencesHooks([]()
    {
        POINTER_SKYRIMSE(TSetPosition, s_setPosition, 19790);
        POINTER_FALLOUT4(TSetPosition, s_setPosition, 0x14040C060 - 0x140000000);

        POINTER_SKYRIMSE(TRotate, s_rotateX, 19787);
        POINTER_FALLOUT4(TRotate, s_rotateX, 0x14040BE70 - 0x140000000);

        POINTER_SKYRIMSE(TRotate, s_rotateY, 19788);
        POINTER_FALLOUT4(TRotate, s_rotateY, 0x14040BF00 - 0x140000000);

        POINTER_SKYRIMSE(TRotate, s_rotateZ, 19789);
        POINTER_FALLOUT4(TRotate, s_rotateZ, 0x14040BF90 - 0x140000000);

        POINTER_SKYRIMSE(TActorProcess, s_actorProcess, 37356);
        POINTER_FALLOUT4(TActorProcess, s_actorProcess, 0x140D7CEB0 - 0x140000000);

        POINTER_SKYRIMSE(TLockChange, s_lockChange, 19512);
        POINTER_FALLOUT4(TLockChange, s_lockChange, 0x1403EDBA0 - 0x140000000);

        POINTER_SKYRIMSE(TCheckForNewPackage, s_checkForNewPackage, 39114);
        POINTER_FALLOUT4(TCheckForNewPackage, s_checkForNewPackage, 0x140E28F80 - 0x140000000);

        POINTER_SKYRIMSE(TInitFromPackage, s_initFromPackage, 38959);
        POINTER_FALLOUT4(TInitFromPackage, s_initFromPackage, 0x140E219A0 - 0x140000000);

        POINTER_SKYRIMSE(TSpeakSoundFunction, s_speakSoundFunction, 37542);

        RealSetPosition = s_setPosition.Get();
        RealRotateX = s_rotateX.Get();
        RealRotateY = s_rotateY.Get();
        RealRotateZ = s_rotateZ.Get();
        RealActorProcess = s_actorProcess.Get();
        RealLockChange = s_lockChange.Get();
        RealCheckForNewPackage = s_checkForNewPackage.Get();
        RealInitFromPackage = s_initFromPackage.Get();
        RealSpeakSoundFunction = s_speakSoundFunction.Get();

        TP_HOOK(&RealSetPosition, HookSetPosition);
        TP_HOOK(&RealRotateX, HookRotateX);
        TP_HOOK(&RealRotateY, HookRotateY);
        TP_HOOK(&RealRotateZ, HookRotateZ);
        TP_HOOK(&RealActorProcess, HookActorProcess);
        TP_HOOK(&RealLockChange, HookLockChange);
        TP_HOOK(&RealCheckForNewPackage, HookCheckForNewPackage);
        TP_HOOK(&RealInitFromPackage, HookInitFromPackage);
        TP_HOOK(&RealSpeakSoundFunction, HookSpeakSoundFunction);
    });

