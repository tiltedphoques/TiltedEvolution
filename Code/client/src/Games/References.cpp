#include <Games/References.h>

#include <Games/Skyrim/RTTI.h>

#include <Games/Skyrim/Forms/TESWorldSpace.h>
#include <Games/Skyrim/Forms/TESObjectCELL.h>
#include <Games/Skyrim/Forms/BGSHeadPart.h>
#include <Games/Skyrim/Forms/TESNPC.h>
#include <Games/SaveLoad.h>

#include <Games/Fallout4/Forms/TESWorldSpace.h>
#include <Games/Fallout4/Forms/TESObjectCELL.h>
#include <Games/Fallout4/Forms/TESNPC.h>

#include <Games/Skyrim/BSAnimationGraphManager.h>
#include <Games/Skyrim/Havok/BShkbAnimationGraph.h>
#include <Games/Skyrim/Havok/hkbBehaviorGraph.h>
#include <Games/Skyrim/Havok/hkbVariableValueSet.h>

#include <Structs/AnimationData.h>
#include <Structs/AnimationVariables.h>

#include <Games/Fallout4/BSAnimationGraphManager.h>
#include <Games/Fallout4/Havok/BShkbAnimationGraph.h>
#include <Games/Fallout4/Havok/hkbBehaviorGraph.h>
#include <Games/Fallout4/Havok/hkbVariableValueSet.h>

#include <Games/TES.h>

#include <Serialization.hpp>

static thread_local bool s_recursionBarrier = false;

struct ScopedRecursion
{
    ScopedRecursion() { s_recursionBarrier = true; }
    ~ScopedRecursion() { s_recursionBarrier = false; }

    TP_NOCOPYMOVE(ScopedRecursion);
};

TP_THIS_FUNCTION(TSetPosition, char, Actor, NiPoint3& acPosition);
TP_THIS_FUNCTION(TRotate, void, TESObjectREFR, float aAngle);
TP_THIS_FUNCTION(TActorProcess, char, Actor, float aValue);

static TSetPosition* RealSetPosition = nullptr;
static TRotate* RealRotateX = nullptr;
static TRotate* RealRotateY = nullptr;
static TRotate* RealRotateZ = nullptr;
static TActorProcess* RealActorProcess = nullptr;

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
        if (pManager->animationGraphIndex < pManager->animationGraphs.size)
        {
            const auto pGraph = pManager->animationGraphs.Get(pManager->animationGraphIndex);
            if (pGraph)
            {
                const auto pVariableSet = pGraph->behaviorGraph->animationVariables;

                if (pVariableSet && pVariableSet->size > 255)
                {
                    aVariables.Booleans = 0;

                    for (size_t i = 0; i < AnimationData::s_booleanLookUpTable.size(); ++i)
                    {
                        const auto idx = AnimationData::s_booleanLookUpTable[i];

                        if (pVariableSet->size > idx && pVariableSet->data[idx] != 0)
                            aVariables.Booleans |= (1ull << i);
                    }

                    for (size_t i = 0; i < AnimationData::kFloatCount; ++i)
                    {
                        const auto idx = AnimationData::s_floatLookupTable[i];
                        aVariables.Floats[i] = *reinterpret_cast<float*>(&pVariableSet->data[idx]);
                    }

                    for (size_t i = 0; i < AnimationData::kIntegerCount; ++i)
                    {
                        const auto idx = AnimationData::s_integerLookupTable[i];
                        aVariables.Integers[i] = *reinterpret_cast<uint32_t*>(&pVariableSet->data[idx]);
                    }
                }
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
        if (pManager->animationGraphIndex < pManager->animationGraphs.size)
        {
            const auto pGraph = pManager->animationGraphs.Get(pManager->animationGraphIndex);
            if (pGraph)
            {
                const auto pVariableSet = pGraph->behaviorGraph->animationVariables;

                if (pVariableSet && pVariableSet->size >= 298)
                {
                    for (size_t i = 0; i < AnimationData::s_booleanLookUpTable.size(); ++i)
                    {
                        const auto idx = AnimationData::s_booleanLookUpTable[i];

                        if (pVariableSet->size > idx)
                        {
                            pVariableSet->data[idx] = (aVariables.Booleans & (1ull << i)) != 0;
                        }
                    }

                    for (size_t i = 0; i < AnimationData::kFloatCount; ++i)
                    {
                        const auto idx = AnimationData::s_floatLookupTable[i];
                        *reinterpret_cast<float*>(&pVariableSet->data[idx]) = aVariables.Floats[i];
                    }

                    for (size_t i = 0; i < AnimationData::kIntegerCount; ++i)
                    {
                        const auto idx = AnimationData::s_integerLookupTable[i];
                        *reinterpret_cast<uint32_t*>(&pVariableSet->data[idx]) = aVariables.Integers[i];
                    }
                }
            }
        }

        pManager->Release();
    }
}

String TESObjectREFR::SerializeInventory() const noexcept
{
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
    BGSLoadFormBuffer loadBuffer(1024);
    loadBuffer.SetSize(acData.size());
    loadBuffer.buffer = acData.c_str();
    loadBuffer.formId = 0;
    loadBuffer.form = nullptr;
    
    LoadInventory(&loadBuffer);

}

uint32_t TESObjectREFR::GetCellId() const noexcept
{
    if (!parentCell)
        return 0;

    const auto pWorldSpace = parentCell->worldspace;

    return pWorldSpace != nullptr ? pWorldSpace->formID : parentCell->formID;
}

TESWorldSpace* TESObjectREFR::GetWorldSpace() const noexcept
{
    auto pParentCell = parentCell ? parentCell : GetParentCell();
    if (pParentCell && !(pParentCell->cellFlags[0] & 1))
        return pParentCell->worldspace;

    return nullptr;
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
    ScopedRecursion recursionGuard;

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

void Actor::GenerateFace() noexcept
{
#ifdef TP_SKYRIM
    auto pFaceNode = GetFaceGenNiNode();
    auto pNpc = RTTI_CAST(baseForm, TESForm, TESNPC);

    if (pFaceNode && pNpc)
    {
        auto pFacePart = pNpc->GetHeadPart(1);
        if (pFacePart)
        {
            NiAVObject* pHeadNode = pFaceNode->GetByName(pFacePart->name);
            if (pHeadNode)
            {
                NiTriBasedGeom* pGeometry = pHeadNode->CastToNiTriBasedGeom();
            }
        }
    }
#elif TP_FALLOUT4
    // Nothing to, fallout 4 is smarter than skyrim
#endif

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
    if (formType == Actor::Type && this != PlayerCharacter::Get())
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

    if (bIsRemote && !s_recursionBarrier)
        return 1;

    // Don't interfere with non actor references, or the player, or if we are calling our self
    if (apThis->formType != Actor::Type || apThis == PlayerCharacter::Get() || s_recursionBarrier)
        return ThisCall(RealSetPosition, apThis, aPosition);

    ScopedRecursion recursionGuard;

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

        RealSetPosition = s_setPosition.Get();
        RealRotateX = s_rotateX.Get();
        RealRotateY = s_rotateY.Get();
        RealRotateZ = s_rotateZ.Get();
        RealActorProcess = s_actorProcess.Get();

        TP_HOOK(&RealSetPosition, HookSetPosition);
        TP_HOOK(&RealRotateX, HookRotateX);
        TP_HOOK(&RealRotateY, HookRotateY);
        TP_HOOK(&RealRotateZ, HookRotateZ);
        TP_HOOK(&RealActorProcess, HookActorProcess);
    });

