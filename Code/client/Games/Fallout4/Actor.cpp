#include <ExtraData/ExtraFactionChanges.h>
#include <Forms/TESFaction.h>
#include <Forms/TESNPC.h>
#include <Games/Memory.h>
#include <Games/References.h>
#include <PlayerCharacter.h>
#include <Games/Fallout4/EquipManager.h>
#include <Forms/BGSObjectInstance.h>
#include <Games/Misc/ActorKnowledge.h>
#include <ExtraData/ExtraDataList.h>

#include <Services/PapyrusService.h>
#include <World.h>

#include <Effects/ValueModifierEffect.h>

#include <Events/HealthChangeEvent.h>
#include <Events/DialogueEvent.h>

#include <Games/Overrides.h>

TP_THIS_FUNCTION(TActorConstructor, Actor*, Actor, uint8_t aUnk);
TP_THIS_FUNCTION(TActorConstructor2, Actor*, Actor, volatile int** aRefCount, uint8_t aUnk);
TP_THIS_FUNCTION(TActorDestructor, void*, Actor);

static TActorConstructor* RealActorConstructor;
static TActorConstructor2* RealActorConstructor2;
static TActorDestructor* RealActorDestructor;

Actor* TP_MAKE_THISCALL(HookActorContructor, Actor, uint8_t aUnk)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    const auto pActor = ThisCall(RealActorConstructor, apThis, aUnk);

    return pActor;
}

Actor* TP_MAKE_THISCALL(HookActorContructor2, Actor, volatile int** aRefCount, uint8_t aUnk)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    const auto pActor = ThisCall(RealActorConstructor2, apThis, aRefCount, aUnk);

    return pActor;
}

void* TP_MAKE_THISCALL(HookActorDestructor, Actor)
{
    // TODO: Actor dtor sometimes has garbage actor, causing a crash
    return ThisCall(RealActorDestructor, apThis);
}

GamePtr<Actor> Actor::New() noexcept
{
    const auto pActor = Memory::Allocate<Actor>();

    ThisCall(HookActorContructor, pActor, uint8_t(1));

    return pActor;
}

TESForm* Actor::GetEquippedWeapon(uint32_t aSlotId) const noexcept
{
    using TGetEquippedWeapon = TESForm*(__fastcall)(void*, void*, const Actor*, uint32_t);

    POINTER_FALLOUT4(TGetEquippedWeapon, s_getEquippedWeapon, 811140);

    return s_getEquippedWeapon(nullptr, nullptr, this, aSlotId);
}

Factions Actor::GetFactions() const noexcept
{
    Factions result;

    auto& modSystem = World::Get().GetModSystem();

    auto* pNpc = Cast<TESNPC>(baseForm);
    if (pNpc)
    {
        auto& factions = pNpc->actorData.factions;

        for (auto i = 0u; i < factions.length; ++i)
        {
            Faction faction;

            modSystem.GetServerModId(factions[i].faction->formID, faction.Id);
            faction.Rank = factions[i].rank;

            result.NpcFactions.push_back(faction);
        }
    }

    auto* pFactionExtras = Cast<ExtraFactionChanges>(extraData->GetByType(ExtraDataType::Faction));
    if (pFactionExtras)
    {
        for (auto i = 0u; i < pFactionExtras->entries.length; ++i)
        {
            Faction faction;

            modSystem.GetServerModId(pFactionExtras->entries[i].faction->formID, faction.Id);
            faction.Rank = pFactionExtras->entries[i].rank;

            result.ExtraFactions.push_back(faction);
        }
    }

    return result;
}

ActorValues Actor::GetEssentialActorValues() noexcept
{
    ActorValues actorValues;

    int essentialValues[] = {ActorValueInfo::kHealth};

    for (auto i : essentialValues)
    {
        ActorValueInfo* pActorValueInfo = GetActorValueInfo(i);
        float value = actorValueOwner.GetValue(pActorValueInfo);
        actorValues.ActorValuesList.insert({i, value});
        float maxValue = actorValueOwner.GetMaxValue(pActorValueInfo);
        actorValues.ActorMaxValuesList.insert({i, maxValue});
    }

    ActorValueInfo* pActorValueInfoRads = GetActorValueInfo(ActorValueInfo::kRads);
    float valueRads = actorValueOwner.GetValue(pActorValueInfoRads);
    actorValues.ActorValuesList.insert({ActorValueInfo::kRads, valueRads});
    ActorValueInfo* pActorValueInfoRadsMax = GetActorValueInfo(ActorValueInfo::kRadHealthMax);
    float valueRadsMax = actorValueOwner.GetValue(pActorValueInfoRadsMax);
    actorValues.ActorValuesList.insert({ActorValueInfo::kRadHealthMax, valueRadsMax});

    return actorValues;
}

// use this for fallout 4 projectile launching
void* Actor::GetCurrentWeapon(void* apResult, uint32_t aEquipIndex) noexcept
{
    TP_THIS_FUNCTION(TGetCurrentWeapon, void*, Actor, void* apResult, uint32_t aEquipIndex);
    POINTER_FALLOUT4(TGetCurrentWeapon, getCurrentWeapon, 1277202);
    return ThisCall(getCurrentWeapon, this, apResult, aEquipIndex);
}

float Actor::GetActorValue(uint32_t aId) const noexcept
{
    ActorValueInfo* pActorValueInfo = GetActorValueInfo(aId);
    return actorValueOwner.GetValue(pActorValueInfo);
}

float Actor::GetActorPermanentValue(uint32_t aId) const noexcept
{
    ActorValueInfo* pActorValueInfo = GetActorValueInfo(aId);
    return actorValueOwner.GetMaxValue(pActorValueInfo);
}

Inventory Actor::GetActorInventory() const noexcept
{
    return GetInventory();
}

Inventory Actor::GetEquipment() const noexcept
{
    Inventory inventory = GetInventory();
    inventory.RemoveByFilter([](const auto& entry) { return !entry.IsWorn(); });
    return inventory;
}

void Actor::SetActorValue(uint32_t aId, float aValue) noexcept
{
    ActorValueInfo* pActorValueInfo = GetActorValueInfo(aId);
    actorValueOwner.SetValue(pActorValueInfo, aValue);
}

void Actor::ForceActorValue(ActorValueOwner::ForceMode aMode, uint32_t aId, float aValue) noexcept
{
    const float current = GetActorValue(aId);
    ActorValueInfo* pActorValueInfo = GetActorValueInfo(aId);
    actorValueOwner.ForceCurrent(aMode, pActorValueInfo, aValue - current);
}

void Actor::SetActorValues(const ActorValues& acActorValues) noexcept
{
    for (auto& value : acActorValues.ActorMaxValuesList)
    {
        ActorValueInfo* pActorValueInfo = GetActorValueInfo(value.first);
        float current = actorValueOwner.GetValue(pActorValueInfo);
        actorValueOwner.ForceCurrent(ActorValueOwner::ForceMode::PERMANENT, pActorValueInfo, value.second - current);
    }

    for (auto& value : acActorValues.ActorValuesList)
    {
        ActorValueInfo* pActorValueInfo = GetActorValueInfo(value.first);
        if (value.first == ActorValueInfo::kRads || value.first == ActorValueInfo::kRadHealthMax)
            actorValueOwner.SetValue(pActorValueInfo, value.second);
        float current = actorValueOwner.GetValue(pActorValueInfo);
        actorValueOwner.ForceCurrent(ActorValueOwner::ForceMode::DAMAGE, pActorValueInfo, value.second - current);
    }
}

void Actor::SetFactions(const Factions& acFactions) noexcept
{
    RemoveFromAllFactions();

    auto& modSystem = World::Get().GetModSystem();

    for (const auto& entry : acFactions.NpcFactions)
    {
        auto* pForm = GetById(modSystem.GetGameId(entry.Id));
        auto* pFaction = Cast<TESFaction>(pForm);
        if (pFaction)
        {
            SetFactionRank(pFaction, entry.Rank);
        }
    }

    for (const auto& entry : acFactions.ExtraFactions)
    {
        auto* pForm = GetById(modSystem.GetGameId(entry.Id));
        auto* pFaction = Cast<TESFaction>(pForm);
        if (pFaction)
        {
            SetFactionRank(pFaction, entry.Rank);
        }
    }
}

void Actor::SetFactionRank(const TESFaction* acpFaction, int8_t aRank) noexcept
{
    PAPYRUS_FUNCTION(void, Actor, SetFactionRank, const TESFaction*, int8_t);
    s_pSetFactionRank(this, acpFaction, aRank);
}

void Actor::SetActorInventory(const Inventory& acInventory) noexcept
{
    spdlog::info("Setting actor inventory, form id: {:X}", formID);

    UnEquipAll();

    SetInventory(acInventory);
}

void Actor::UnEquipAll() noexcept
{
    TP_THIS_FUNCTION(TUnEquipAll, void, Actor);
    POINTER_FALLOUT4(TUnEquipAll, s_unequipAll, 1260318);
    ThisCall(s_unequipAll, this);
}

void Actor::RemoveFromAllFactions() noexcept
{
    PAPYRUS_FUNCTION(void, Actor, RemoveFromAllFactions);
    s_pRemoveFromAllFactions(this);
}

bool Actor::IsDead() noexcept
{
    PAPYRUS_FUNCTION(bool, Actor, IsDead);
    return s_pIsDead(this);
}

void Actor::Kill() noexcept
{
    PAPYRUS_FUNCTION(void, Actor, Kill, void*);
    s_pKill(this, NULL);
}

void Actor::Reset() noexcept
{
    using ObjectReference = TESObjectREFR;
    PAPYRUS_FUNCTION(void, ObjectReference, Reset, int, TESObjectREFR*);
    s_pReset(this, 0, nullptr);
}

void Actor::Respawn() noexcept
{
    Resurrect(false);
    Reset();
}

// TODO(cosideci): this is flawed, since we need to equip specific stacks.
void Actor::ProcessScriptedEquip(TESBoundObject* apObj, bool abEquipLockState, bool abSilent) noexcept
{
    ScopedEquipOverride _;

    TP_THIS_FUNCTION(TProcessScriptedEquip, void, Actor, TESBoundObject*, bool, bool);
    POINTER_FALLOUT4(TProcessScriptedEquip, processScriptedEquip, 868003);
    ThisCall(processScriptedEquip, this, apObj, abEquipLockState, abSilent);
}

void Actor::DropOrPickUpObject(const Inventory::Entry& arEntry, NiPoint3* apPoint, NiPoint3* apRotate) noexcept
{
    ModSystem& modSystem = World::Get().GetModSystem();

    uint32_t objectId = modSystem.GetGameId(arEntry.BaseId);
    TESBoundObject* pObject = Cast<TESBoundObject>(TESForm::GetById(objectId));
    if (!pObject)
    {
        spdlog::warn("Object to drop not found, {:X}:{:X}.", arEntry.BaseId.ModId,
                     arEntry.BaseId.BaseId);
        return;
    }

    if (arEntry.Count < 0)
        DropObject(pObject, -arEntry.Count, apPoint, apRotate);
    // TODO: pick up
}

void Actor::DropObject(TESBoundObject* apObject, int32_t aCount, NiPoint3* apPoint, NiPoint3* apRotate) noexcept
{
    BGSObjectInstance object(apObject, nullptr);

    TP_THIS_FUNCTION(TDropObject, BSPointerHandle<TESObjectREFR>*, Actor, BSPointerHandle<TESObjectREFR>*,
                     BGSObjectInstance*, void*, int32_t, NiPoint3*, NiPoint3*);
    POINTER_FALLOUT4(TDropObject, dropObject, 1482294);

    BSPointerHandle<TESObjectREFR> result{};
    ThisCall(dropObject, this, &result, &object, nullptr, aCount, apPoint, apRotate);
}

void Actor::UnequipItem(TESBoundObject* apObject) noexcept
{
    ScopedEquipOverride _;

    PAPYRUS_FUNCTION(void, Actor, UnequipItem, TESBoundObject*, bool, bool);
    s_pUnequipItem(this, apObject, false, true);
}

TP_THIS_FUNCTION(TDamageActor, bool, Actor, float aDamage, Actor* apHitter);
static TDamageActor* RealDamageActor = nullptr;

bool TP_MAKE_THISCALL(HookDamageActor, Actor, float aDamage, Actor* apHitter)
{
    const auto pExHittee = apThis->GetExtension();
    if (pExHittee->IsLocalPlayer())
    {
        World::Get().GetRunner().Trigger(HealthChangeEvent(apThis->formID, -aDamage));
        return ThisCall(RealDamageActor, apThis, aDamage, apHitter);
    }
    else if (pExHittee->IsRemotePlayer())
    {
        return false;
    }

    if (apHitter)
    {
        const auto pExHitter = apHitter->GetExtension();
        if (pExHitter->IsLocalPlayer())
        {
            World::Get().GetRunner().Trigger(HealthChangeEvent(apThis->formID, -aDamage));
            return ThisCall(RealDamageActor, apThis, aDamage, apHitter);
        }
        if (pExHitter->IsRemotePlayer())
        {
            return false;
        }
    }

    if (pExHittee->IsLocal())
    {
        World::Get().GetRunner().Trigger(HealthChangeEvent(apThis->formID, -aDamage));
        return ThisCall(RealDamageActor, apThis, aDamage, apHitter);
    }
    else
    {
        return false;
    }
}

TP_THIS_FUNCTION(TApplyActorEffect, void, ActiveEffect, Actor* apTarget, float aEffectValue,
                 ActorValueInfo* apActorValueInfo);
static TApplyActorEffect* RealApplyActorEffect = nullptr;

void TP_MAKE_THISCALL(HookApplyActorEffect, ActiveEffect, Actor* apTarget, float aEffectValue,
                      ActorValueInfo* apActorValueInfo)
{
    const auto* pValueModEffect = Cast<ValueModifierEffect>(apThis);

    if (pValueModEffect)
    {
        ActorValueInfo* pHealthActorValueInfo = apTarget->GetActorValueInfo(ActorValueInfo::kHealth);
        if (pValueModEffect->actorValueInfo == pHealthActorValueInfo && aEffectValue > 0.0f)
        {
            if (apTarget && apTarget->GetExtension())
            {
                const auto pExTarget = apTarget->GetExtension();
                if (pExTarget->IsLocal())
                {
                    World::Get().GetRunner().Trigger(HealthChangeEvent(apTarget->formID, aEffectValue));
                    return ThisCall(RealApplyActorEffect, apThis, apTarget, aEffectValue, apActorValueInfo);
                }
                return;
            }
        }
    }

    return ThisCall(RealApplyActorEffect, apThis, apTarget, aEffectValue, apActorValueInfo);
}

TP_THIS_FUNCTION(TRunDetection, void, void, ActorKnowledge*);
static TRunDetection* RealRunDetection = nullptr;

void TP_MAKE_THISCALL(HookRunDetection, void, ActorKnowledge* apTarget)
{
    auto pOwner = TESObjectREFR::GetByHandle(apTarget->hOwner);
    auto pTarget = TESObjectREFR::GetByHandle(apTarget->hTarget);

    if (pOwner && pTarget)
    {
        auto pOwnerActor = Cast<Actor>(pOwner);
        auto pTargetActor = Cast<Actor>(pTarget);
        if (pOwnerActor && pTargetActor)
        {
            if (pOwnerActor->GetExtension()->IsRemotePlayer() && pTargetActor->GetExtension()->IsLocalPlayer())
            {
                spdlog::info("Cancelling detection from remote player to local player");
                return;
            }
        }
    }

    return ThisCall(RealRunDetection, apThis, apTarget);
}

TP_THIS_FUNCTION(TSpeakSoundFunction, float, Actor, const char* apName, uint32_t* aSoundHand, void* apArchTypeAnimation, int32_t aiStringLength, bool abSetEmotion, void* apOutputModel, bool abQueue, bool abLip, bool abPCapcall);
static TSpeakSoundFunction* RealSpeakSoundFunction = nullptr;

bool TP_MAKE_THISCALL(HookSpeakSoundFunction, Actor, const char* apName, uint32_t* aSoundHand, void* apArchTypeAnimation, int32_t aiStringLength, bool abSetEmotion, void* apOutputModel, bool abQueue, bool abLip, bool abPCapcall)
{
    if (apThis->GetExtension()->IsLocal())
        World::Get().GetRunner().Trigger(DialogueEvent(apThis->formID, apName));

    return ThisCall(RealSpeakSoundFunction, apThis, apName, aSoundHand, apArchTypeAnimation, aiStringLength, abSetEmotion, apOutputModel, abQueue, abLip, abPCapcall);
}

void Actor::SpeakSound(const char* pFile)
{
    uint32_t handle[4]{};
    handle[0] = -1;
    ThisCall(RealSpeakSoundFunction, this, pFile, handle, nullptr, 0, false, nullptr, false, true, false);
}

static TiltedPhoques::Initializer s_specificReferencesHooks([]() {
    POINTER_FALLOUT4(TActorConstructor, s_actorCtor, 1027501);
    POINTER_FALLOUT4(TActorConstructor2, s_actorCtor2, 1331729);
    POINTER_FALLOUT4(TActorDestructor, s_actorDtor, 1104083);
    POINTER_FALLOUT4(TDamageActor, s_damageActor, 1539011);
    // TODO: not sure about this ID, seems to interfere with jump when hooked?
    POINTER_FALLOUT4(TApplyActorEffect, s_applyActorEffect, 703727);
    POINTER_FALLOUT4(TRunDetection, s_runDetection, 906785);
    POINTER_FALLOUT4(TSpeakSoundFunction, s_speakSoundFunction, 1567997);

    RealActorConstructor = s_actorCtor.Get();
    RealActorConstructor2 = s_actorCtor2.Get();
    RealActorDestructor = s_actorDtor.Get();
    RealDamageActor = s_damageActor.Get();
    RealApplyActorEffect = s_applyActorEffect.Get();
    RealRunDetection = s_runDetection.Get();
    RealSpeakSoundFunction = s_speakSoundFunction.Get();

    TP_HOOK(&RealActorConstructor, HookActorContructor);
    TP_HOOK(&RealActorConstructor2, HookActorContructor2);
    TP_HOOK(&RealActorDestructor, HookActorDestructor);
    TP_HOOK(&RealDamageActor, HookDamageActor);
    TP_HOOK(&RealApplyActorEffect, HookApplyActorEffect);
    TP_HOOK(&RealRunDetection, HookRunDetection);
    TP_HOOK(&RealSpeakSoundFunction, HookSpeakSoundFunction);
});
