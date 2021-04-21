#include <TiltedOnlinePCH.h>

#include <Games/References.h>

#include <RTTI.h>

#include <Forms/TESWorldSpace.h>
#include <Forms/TESObjectCELL.h>
#include <Forms/BGSHeadPart.h>
#include <Forms/TESNPC.h>
#include <SaveLoad.h>

#include <Events/ActivateEvent.h>

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

#include <TiltedCore/Serialization.hpp>

#include <Services/PapyrusService.h>
#include <World.h>

using ScopedReferencesOverride = ScopedOverride<TESObjectREFR>;
thread_local uint32_t ScopedReferencesOverride::s_refCount = 0;

TP_THIS_FUNCTION(TSetPosition, char, Actor, NiPoint3& acPosition);
TP_THIS_FUNCTION(TRotate, void, TESObjectREFR, float aAngle);
TP_THIS_FUNCTION(TActorProcess, char, Actor, float aValue);
TP_THIS_FUNCTION(TActivate, void, TESObjectREFR, TESObjectREFR* apActivator, uint8_t aUnk1, int64_t aUnk2, int aUnk3, char aUnk4);

static TSetPosition* RealSetPosition = nullptr;
static TRotate* RealRotateX = nullptr;
static TRotate* RealRotateY = nullptr;
static TRotate* RealRotateZ = nullptr;
static TActorProcess* RealActorProcess = nullptr;
static TActivate* RealActivate = nullptr;

TESObjectREFR* TESObjectREFR::GetByHandle(uint32_t aHandle) noexcept
{
    TESObjectREFR* pResult = nullptr;

    using TGetRefrByHandle = void(uint32_t& aHandle, TESObjectREFR*& apResult);

    POINTER_SKYRIMSE(TGetRefrByHandle, s_getRefrByHandle, 0x1402130F0 - 0x140000000);
    POINTER_FALLOUT4(TGetRefrByHandle, s_getRefrByHandle, 0x140023740 - 0x140000000);

    s_getRefrByHandle.Get()(aHandle, pResult);

    if (pResult)
        pResult->handleRefObject.DecRefHandle();

    return pResult;
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
            const auto* pGraph = pManager->animationGraphs.Get(pManager->animationGraphIndex);

            if (!pGraph)
                return;
        
            if (!pGraph->behaviorGraph || !pGraph->behaviorGraph->stateMachine ||
                !pGraph->behaviorGraph->stateMachine->name)
                return;

            const auto* pDescriptor =
                AnimationGraphDescriptorManager::Get().GetDescriptor(pGraph->behaviorGraph->stateMachine->name);

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

            const auto* pDescriptor =
                AnimationGraphDescriptorManager::Get().GetDescriptor(pGraph->behaviorGraph->stateMachine->name);

            if (!pDescriptor)
            {
                if ((formID & 0xFF000000) == 0xFF000000)
                    spdlog::info("Form id {} has {}", formID, pGraph->behaviorGraph->stateMachine->name);
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

    char buffer[1 << 15];
    BGSSaveFormBuffer saveBuffer;
    saveBuffer.buffer = buffer;
    saveBuffer.capacity = 1 << 15;
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

BSExtraDataList* TESObjectREFR::GetExtraDataList() noexcept
{
#if TP_FALLOUT4
    return extraData;
#else
    return &extraData;
#endif
}

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

void TESObjectREFR::MoveTo(TESObjectCELL* apCell, const NiPoint3& acPosition) const noexcept
{
    ScopedReferencesOverride recursionGuard;

    TP_THIS_FUNCTION(TInternalMoveTo, bool, const TESObjectREFR, uint32_t*&, TESObjectCELL*, TESWorldSpace*,
                     const NiPoint3&, const NiPoint3&);

    POINTER_SKYRIMSE(TInternalMoveTo, s_internalMoveTo, 0x1409AE5C0 - 0x140000000);
    POINTER_FALLOUT4(TInternalMoveTo, s_internalMoveTo, 0x1413FE7E0 - 0x140000000);

    ThisCall(s_internalMoveTo, this, s_nullHandle.Get(), apCell, apCell->worldspace, acPosition, rotation);
}

void TESObjectREFR::Activate(TESObjectREFR* apActivator, uint8_t aUnk1, int64_t aUnk2, int aUnk3, char aUnk4) noexcept
{
    return ThisCall(RealActivate, this, apActivator, aUnk1, aUnk2, aUnk3, aUnk4);
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

    POINTER_SKYRIMSE(TQueueUpdate, QueueUpdate, 0x140693110 - 0x140000000);
    POINTER_FALLOUT4(TQueueUpdate, QueueUpdate, 0x140D8A1F0 - 0x140000000);

#ifdef TP_SKYRIM
    ThisCall(QueueUpdate, this, true);
#else
    ThisCall(QueueUpdate, this, true, 0, true, 0);
#endif

    pSetting->data = originalValue;
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
    pActor->CreateMagicCaster(0);
    pActor->CreateMagicCaster(1);
    pActor->CreateMagicCaster(2);
#endif

    pActor->flags &= 0xFFDFFFFF;

    return pActor;
}


void Actor::SetLevelMod(uint32_t aLevel) noexcept
{
    TP_THIS_FUNCTION(TActorSetLevelMod, void, BSExtraDataList, uint32_t);
    POINTER_SKYRIMSE(TActorSetLevelMod, realSetLevelMod, 0x119450);
    POINTER_FALLOUT4(TActorSetLevelMod, realSetLevelMod, 0x8F660);

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
    POINTER_SKYRIMSE(PlayerCharacter*, s_character, 0x142EFF7D8 - 0x140000000);

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

void TP_MAKE_THISCALL(HookActivate, TESObjectREFR, TESObjectREFR* apActivator, uint8_t aUnk1, int64_t aUnk2, int aUnk3, char aUnk4)
{
    auto* pActivator = RTTI_CAST(apActivator, TESObjectREFR, Actor);
    if (pActivator)
        World::Get().GetRunner().Trigger(ActivateEvent(apThis, pActivator, aUnk1, aUnk2, aUnk3, aUnk4));

    return ThisCall(RealActivate, apThis, apActivator, aUnk1, aUnk2, aUnk3, aUnk4);
}

TiltedPhoques::Initializer s_referencesHooks([]()
    {
        POINTER_SKYRIMSE(TSetPosition, s_setPosition, 0x140296910 - 0x140000000);
        POINTER_FALLOUT4(TSetPosition, s_setPosition, 0x14040C060 - 0x140000000);

        POINTER_SKYRIMSE(TRotate, s_rotateX, 0x140296680 - 0x140000000);
        POINTER_FALLOUT4(TRotate, s_rotateX, 0x14040BE70 - 0x140000000);

        POINTER_SKYRIMSE(TRotate, s_rotateY, 0x140296740 - 0x140000000);
        POINTER_FALLOUT4(TRotate, s_rotateY, 0x14040BF00 - 0x140000000);

        POINTER_SKYRIMSE(TRotate, s_rotateZ, 0x140296800 - 0x140000000);
        POINTER_FALLOUT4(TRotate, s_rotateZ, 0x14040BF90 - 0x140000000);

        POINTER_SKYRIMSE(TActorProcess, s_actorProcess, 0x1405D87F0 - 0x140000000);
        POINTER_FALLOUT4(TActorProcess, s_actorProcess, 0x140D7CEB0 - 0x140000000);

        POINTER_SKYRIMSE(TActivate, s_activate, 0x140296C00 - 0x140000000);
        POINTER_FALLOUT4(TActivate, s_activate, 0x14040C750 - 0x140000000);

        RealSetPosition = s_setPosition.Get();
        RealRotateX = s_rotateX.Get();
        RealRotateY = s_rotateY.Get();
        RealRotateZ = s_rotateZ.Get();
        RealActorProcess = s_actorProcess.Get();
        RealActivate = s_activate.Get();

        TP_HOOK(&RealSetPosition, HookSetPosition);
        TP_HOOK(&RealRotateX, HookRotateX);
        TP_HOOK(&RealRotateY, HookRotateY);
        TP_HOOK(&RealRotateZ, HookRotateZ);
        TP_HOOK(&RealActorProcess, HookActorProcess);
        TP_HOOK(&RealActivate, HookActivate);
    });

