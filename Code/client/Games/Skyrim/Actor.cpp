#include <Games/References.h>
#include <Games/Skyrim/EquipManager.h>
#include <AI/AIProcess.h>
#include <Misc/MiddleProcess.h>
#include <Misc/GameVM.h>
#include <DefaultObjectManager.h>
#include <Forms/TESNPC.h>
#include <Forms/TESFaction.h>
#include <Components/TESActorBaseData.h>
#include <ExtraData/ExtraFactionChanges.h>
#include <Games/Memory.h>

#include <Events/HealthChangeEvent.h>
#include <Events/InventoryChangeEvent.h>
#include <Events/MountEvent.h>
#include <Events/DialogueEvent.h>

#include <World.h>
#include <Services/PapyrusService.h>

#include <Forms/ActorValueInfo.h>

#include <Effects/ValueModifierEffect.h>

#include <Games/Skyrim/Misc/InventoryEntry.h>
#include <Games/Skyrim/ExtraData/ExtraCount.h>
#include <Games/Misc/ActorKnowledge.h>

#include <ExtraData/ExtraDataList.h>
#include <ExtraData/ExtraCharge.h>
#include <ExtraData/ExtraCount.h>
#include <ExtraData/ExtraEnchantment.h>
#include <ExtraData/ExtraHealth.h>
#include <ExtraData/ExtraPoison.h>
#include <ExtraData/ExtraSoul.h>
#include <ExtraData/ExtraTextDisplayData.h>
#include <Forms/EnchantmentItem.h>
#include <Forms/AlchemyItem.h>

#include <Structs/Skyrim/AnimationGraphDescriptor_BHR_Master.h>

#include <Games/Overrides.h>
#include <Games/Skyrim/BSAnimationGraphManager.h>
#include <Havok/hkbStateMachine.h>
#include <Havok/hkbBehaviorGraph.h>

#ifdef SAVE_STUFF

#include <Games/Skyrim/SaveLoad.h>
#include "Actor.h"

void Actor::Save_Reversed(const uint32_t aChangeFlags, Buffer::Writer& aWriter)
{
    BGSSaveFormBuffer buffer;

    Save(&buffer);

    AIProcess* pProcess = currentProcess;
    const int32_t handlerId = pProcess != nullptr ? pProcess->handlerId : -1;

    aWriter.WriteBytes((uint8_t*)&handlerId, 4); // TODO: is this needed ?
    aWriter.WriteBytes((uint8_t*)&flags1, 4);

    //     if (!handlerId
//         && (uint8_t)AIProcess::GetBoolInSubStructure(pProcess))
//     {
//         Actor::SaveSkinFar(this);
//     }


    TESObjectREFR::Save_Reversed(aChangeFlags, aWriter);

    if (pProcess); // Skyrim saves the process manager state, but we don't give a shit so skip !

    aWriter.WriteBytes((uint8_t*)&unk194, 4);
    aWriter.WriteBytes((uint8_t*)&headTrackingUpdateDelay, 4);
    aWriter.WriteBytes((uint8_t*)&unk9C, 4);
	// We skip 0x180 as it's not something we care about, some timer related data
   


    aWriter.WriteBytes((uint8_t*)&unk98, 4);
	// skip A8 - related to timers 
	// skip AC - related to timers as well
    aWriter.WriteBytes((uint8_t*)&unkB0, 4);
	// skip E4 - never seen this used
	// skip E8 - same as E4
    aWriter.WriteBytes((uint8_t*)&unk84, 4);
    aWriter.WriteBytes((uint8_t*)&unkA4, 4);
	// skip baseForm->weight
	// skip 12C

	// Save actor state sub_6F0FB0
}

#endif

TP_THIS_FUNCTION(TCharacterConstructor, Actor*, Actor);
TP_THIS_FUNCTION(TCharacterConstructor2, Actor*, Actor, uint8_t aUnk);
TP_THIS_FUNCTION(TCharacterDestructor, Actor*, Actor);
TP_THIS_FUNCTION(TAddInventoryItem, void, Actor, TESBoundObject* apItem, ExtraDataList* apExtraData, int32_t aCount, TESObjectREFR* apOldOwner);
TP_THIS_FUNCTION(TPickUpObject, void*, Actor, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, float aUnk2);
TP_THIS_FUNCTION(TDropObject, void*, Actor, void* apResult, TESBoundObject* apObject, ExtraDataList* apExtraData, int32_t aCount, NiPoint3* apLocation, NiPoint3* apRotation);

using TGetLocation = TESForm *(TESForm *);
static TGetLocation *FUNC_GetActorLocation;

TCharacterConstructor* RealCharacterConstructor;
TCharacterConstructor2* RealCharacterConstructor2;
TCharacterDestructor* RealCharacterDestructor;

static TAddInventoryItem* RealAddInventoryItem = nullptr;
static TPickUpObject* RealPickUpObject = nullptr;
static TDropObject* RealDropObject = nullptr;

Actor* TP_MAKE_THISCALL(HookCharacterConstructor, Actor)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    ThisCall(RealCharacterConstructor, apThis);

    return apThis;
}

Actor* TP_MAKE_THISCALL(HookCharacterConstructor2, Actor, uint8_t aUnk)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    ThisCall(RealCharacterConstructor2, apThis, aUnk);

    return apThis;
}

Actor* TP_MAKE_THISCALL(HookCharacterDestructor, Actor)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    auto pExtension = apThis->GetExtension();

    if(pExtension)
    {
        pExtension->~ActorExtension();
    }

    ThisCall(RealCharacterDestructor, apThis);

    return apThis;
}

GamePtr<Actor> Actor::New() noexcept
{
    auto* const pActor = Memory::Allocate<Actor>();

    ThisCall(RealCharacterConstructor, pActor);

    return {pActor};
}

void Actor::InterruptCast(bool abRefund) noexcept
{
    TP_THIS_FUNCTION(TInterruptCast, void, Actor, bool abRefund);

    POINTER_SKYRIMSE(TInterruptCast, s_interruptCast, 38757);

    ThisCall(s_interruptCast, this, abRefund);
}

TESForm* Actor::GetEquippedWeapon(uint32_t aSlotId) const noexcept
{
    if (currentProcess && currentProcess->middleProcess)
    {
        auto pMiddleProcess = currentProcess->middleProcess;

        if (aSlotId == 0 && pMiddleProcess->leftEquippedObject)
            return pMiddleProcess->leftEquippedObject->pObject;

        else if (aSlotId == 1 && pMiddleProcess->rightEquippedObject)
            return pMiddleProcess->rightEquippedObject->pObject;

    }

    return nullptr;
}

TESForm* Actor::GetEquippedAmmo() const noexcept
{
    if (currentProcess && currentProcess->middleProcess && currentProcess->middleProcess->ammoEquippedObject)
    {
        // TODO: rtti cast to check if is ammo object? or actually, just call AIProcess::GetCurrentAmmo()
        return currentProcess->middleProcess->ammoEquippedObject->pObject;
    }

    return nullptr;
}

TESForm *Actor::GetCurrentLocation()
{
    // we use the safe function which also
    // checks the form type
    return FUNC_GetActorLocation(this);
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

    auto* pChanges = Cast<ExtraFactionChanges>(extraData.GetByType(ExtraDataType::Faction));
    if (pChanges)
    {
        for (auto i = 0u; i < pChanges->entries.length; ++i)
        {
            Faction faction;

            modSystem.GetServerModId(pChanges->entries[i].faction->formID, faction.Id);
            faction.Rank = pChanges->entries[i].rank;

            result.ExtraFactions.push_back(faction);
        }
    }

    return result;
}

ActorValues Actor::GetEssentialActorValues() const noexcept
{
    ActorValues actorValues;

    int essentialValues[] = {ActorValueInfo::kHealth, ActorValueInfo::kStamina, ActorValueInfo::kMagicka};
    for (auto i : essentialValues)
    {
        float value = actorValueOwner.GetValue(i);
        actorValues.ActorValuesList.insert({i, value});
        float maxValue = actorValueOwner.GetPermanentValue(i);
        actorValues.ActorMaxValuesList.insert({i, maxValue});
    }

    return actorValues;
}

float Actor::GetActorValue(uint32_t aId) const noexcept
{
    return actorValueOwner.GetValue(aId);
}

float Actor::GetActorPermanentValue(uint32_t aId) const noexcept
{
    return actorValueOwner.GetPermanentValue(aId);
}

void Actor::SetActorValue(uint32_t aId, float aValue) noexcept
{
    actorValueOwner.SetValue(aId, aValue);
}

void Actor::ForceActorValue(ActorValueOwner::ForceMode aMode, uint32_t aId, float aValue) noexcept
{
    const float current = GetActorValue(aId);
    actorValueOwner.ForceCurrent(aMode, aId, aValue - current);
}

Inventory Actor::GetActorInventory() const noexcept
{
    Inventory inventory = GetInventory();

    inventory.CurrentMagicEquipment = GetMagicEquipment();

    return inventory;
}

MagicEquipment Actor::GetMagicEquipment() const noexcept
{
    MagicEquipment equipment;

    auto& modSystem = World::Get().GetModSystem();

    uint32_t mainId = magicItems[0] ? magicItems[0]->formID : 0;
    modSystem.GetServerModId(mainId, equipment.LeftHandSpell);

    uint32_t secondaryId = magicItems[1] ? magicItems[1]->formID : 0;
    modSystem.GetServerModId(secondaryId, equipment.RightHandSpell);

    uint32_t shoutId = equippedShout ? equippedShout->formID : 0;
    modSystem.GetServerModId(shoutId, equipment.Shout);

    return equipment;
}

Inventory Actor::GetEquipment() const noexcept
{
    Inventory inventory = GetInventory();
    inventory.RemoveByFilter([](const auto& entry) { return !entry.IsWorn(); });
    inventory.CurrentMagicEquipment = GetMagicEquipment();
    return inventory;
}

int32_t Actor::GetGoldAmount() noexcept
{
    TP_THIS_FUNCTION(TGetGoldAmount, int32_t, Actor);
    POINTER_SKYRIMSE(TGetGoldAmount, s_getGoldAmount, 37527);
    return ThisCall(s_getGoldAmount, this);
}

void Actor::SetActorInventory(const Inventory& aInventory) noexcept
{
    spdlog::info("Setting inventory for actor {:X}", formID);

    UnEquipAll();

    SetInventory(aInventory);
    SetMagicEquipment(aInventory.CurrentMagicEquipment);
}

void Actor::SetMagicEquipment(const MagicEquipment& acEquipment) noexcept
{
    auto* pEquipManager = EquipManager::Get();
    auto& modSystem = World::Get().GetModSystem();

    if (acEquipment.LeftHandSpell)
    {
        uint32_t mainHandWeaponId = modSystem.GetGameId(acEquipment.LeftHandSpell);
        spdlog::debug("Setting left hand spell: {:X}", mainHandWeaponId);
        pEquipManager->EquipSpell(this, TESForm::GetById(mainHandWeaponId), 0);
    }

    if (acEquipment.RightHandSpell)
    {
        uint32_t secondaryHandWeaponId = modSystem.GetGameId(acEquipment.RightHandSpell);
        spdlog::debug("Setting right hand spell: {:X}", secondaryHandWeaponId);
        pEquipManager->EquipSpell(this, TESForm::GetById(secondaryHandWeaponId), 1);
    }

    if (acEquipment.Shout)
    {
        uint32_t shoutId = modSystem.GetGameId(acEquipment.Shout);
        spdlog::debug("Setting shout: {:X}", shoutId);
        pEquipManager->EquipShout(this, TESForm::GetById(shoutId));
    }
}

void Actor::SetActorValues(const ActorValues& acActorValues) noexcept
{
    for (auto& value : acActorValues.ActorMaxValuesList)
    {
        float current = actorValueOwner.GetValue(value.first);
        actorValueOwner.ForceCurrent(ActorValueOwner::ForceMode::PERMANENT, value.first, value.second - current);
    }

    for (auto& value : acActorValues.ActorValuesList)
    {
        float current = actorValueOwner.GetValue(value.first);
        actorValueOwner.ForceCurrent(ActorValueOwner::ForceMode::DAMAGE, value.first, value.second - current);
    }
}

void Actor::SetFactions(const Factions& acFactions) noexcept
{
    RemoveFromAllFactions();

    auto& modSystem = World::Get().GetModSystem();

    for (auto& entry : acFactions.NpcFactions)
    {
        auto pForm = GetById(modSystem.GetGameId(entry.Id));
        auto pFaction = Cast<TESFaction>(pForm);
        if (pFaction)
        {
            SetFactionRank(pFaction, entry.Rank);
        }
    }

    for (auto& entry : acFactions.ExtraFactions)
    {
        auto pForm = GetById(modSystem.GetGameId(entry.Id));
        auto pFaction = Cast<TESFaction>(pForm);
        if (pFaction)
        {
            SetFactionRank(pFaction, entry.Rank);
        }
    }
}

void Actor::SetFactionRank(const TESFaction* apFaction, int8_t aRank) noexcept
{
    TP_THIS_FUNCTION(TSetFactionRankInternal, void, Actor, const TESFaction*, int8_t);

    POINTER_SKYRIMSE(TSetFactionRankInternal, s_setFactionRankInternal, 37677);

    ThisCall(s_setFactionRankInternal, this, apFaction, aRank);
}

void Actor::SetNoBleedoutRecovery(bool aSet) noexcept
{
    TP_THIS_FUNCTION(TSetNoBleedoutRecovery, void, Actor, bool);
    POINTER_SKYRIMSE(TSetNoBleedoutRecovery, s_setNoBleedoutRecovery, 38533);
    ThisCall(s_setNoBleedoutRecovery, this, aSet);
}

void Actor::SetPlayerRespawnMode() noexcept
{
    SetEssentialEx(true);
    // Makes the player go in an unrecoverable bleedout state
    SetNoBleedoutRecovery(true);

    if (formID != 0x14)
    {
        //SetPlayerTeammate(true);

        auto pPlayerFaction = Cast<TESFaction>(TESForm::GetById(0xDB1));
        SetFactionRank(pPlayerFaction, 1);
    }
}

void Actor::SetPlayerTeammate(bool aSet) noexcept
{
    TP_THIS_FUNCTION(TSetPlayerTeammate, void, Actor, bool aSet, bool abCanDoFavor);
    POINTER_SKYRIMSE(TSetPlayerTeammate, setPlayerTeammate, 37717);
    return ThisCall(setPlayerTeammate, this, aSet, true);
}

void Actor::UnEquipAll() noexcept
{
    // For each change 
    const auto pContainerChanges = GetContainerChanges()->entries;
    for (auto pChange : *pContainerChanges)
    {
        if (pChange && pChange->form && pChange->dataList)
        {
            // Parse all extra data lists
            const auto pDataLists = pChange->dataList;
            for (auto* pDataList : *pDataLists)
            {
                if (pDataList)
                {
                    BSScopedLock<BSRecursiveLock> _(pDataList->lock);

                    // Right slot
                    if (pDataList->Contains(ExtraDataType::Worn))
                    {
                        EquipManager::Get()->UnEquip(this, pChange->form, pDataList, 1, DefaultObjectManager::Get().rightEquipSlot, false, true, false, false, nullptr);
                    }

                    // Left slot
                    if (pDataList->Contains(ExtraDataType::WornLeft))
                    {
                        EquipManager::Get()->UnEquip(this, pChange->form, pDataList, 1, DefaultObjectManager::Get().leftEquipSlot, false, true, false, false, nullptr);
                    }
                }
            }
        }
    }

    // Taken from skyrim's code shouts can be two form types apparently
    if (equippedShout && ((int)equippedShout->formType - 41) <= 1)
    {
        EquipManager::Get()->UnEquipShout(this, equippedShout);
        equippedShout = nullptr;
    }
}

void Actor::RemoveFromAllFactions() noexcept
{
    PAPYRUS_FUNCTION(void, Actor, RemoveFromAllFactions);

    s_pRemoveFromAllFactions(this);
}

TP_THIS_FUNCTION(TInitiateMountPackage, bool, Actor, Actor* apMount);
static TInitiateMountPackage* RealInitiateMountPackage = nullptr;

bool Actor::InitiateMountPackage(Actor* apMount) noexcept
{
    return ThisCall(RealInitiateMountPackage, this, apMount);
}

void Actor::GenerateMagicCasters() noexcept
{
    using CS = MagicSystem::CastingSource;

    for (int i = 0; i < 4; i++)
    {
        if (casters[i] == nullptr)
            casters[i] = Cast<ActorMagicCaster>(GetMagicCaster(static_cast<CS>(i)));
    }
}

bool Actor::IsDead() noexcept
{
    PAPYRUS_FUNCTION(bool, Actor, IsDead);

    return s_pIsDead(this);
}

bool Actor::IsDragon() noexcept
{
    // TODO: if anyone has a better way of doing this, please do tell.
    BSAnimationGraphManager* pManager = nullptr;
    animationGraphHolder.GetBSAnimationGraph(&pManager);

    if (!pManager)
        return false;

    const auto* pGraph = pManager->animationGraphs.Get(pManager->animationGraphIndex);
    if (!pGraph)
        return false;

    return AnimationGraphDescriptor_BHR_Master::m_key == pManager->GetDescriptorKey();
}

void Actor::Kill() noexcept
{
    // Never kill players
    ActorExtension* pExtension = GetExtension();
    if (pExtension->IsPlayer())
        return;

    // TODO: these args are kind of bogus of course
    KillImpl(nullptr, 100.f, true, true);

    // Papyrus kill will not go through if it is queued by a kill move
    /*
    PAPYRUS_FUNCTION(void, Actor, Kill, void*);
    s_pKill(this, NULL);
    */
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

TP_THIS_FUNCTION(TForceState, void, Actor, const NiPoint3&, float, float, TESObjectCELL*, TESWorldSpace*, bool);
static TForceState* RealForceState = nullptr;

void TP_MAKE_THISCALL(HookForceState, Actor, const NiPoint3& acPosition, float aX, float aZ,
                      TESObjectCELL* apCell, TESWorldSpace* apWorldSpace, bool aUnkBool)
{
    /*const auto pNpc = Cast<TESNPC>(apThis->baseForm);
    if (pNpc)
    {
        spdlog::info("For TESNPC: {}, spawn at {} {} {}", pNpc->fullName.value, apPosition->m_x, apPosition->m_y,
                     apPosition->m_z);
    }*/

   // if (apThis != PlayerCharacter::Get())
   //     return;

    return ThisCall(RealForceState, apThis, acPosition, aX, aZ, apCell, apWorldSpace, aUnkBool);
}

TP_THIS_FUNCTION(TSpawnActorInWorld, bool, Actor);
static TSpawnActorInWorld* RealSpawnActorInWorld = nullptr;

// TODO: this isn't SpawnActorInWorld, this is TESObjectREFR::UpdateReference3D()
bool TP_MAKE_THISCALL(HookSpawnActorInWorld, Actor)
{
    const auto* pNpc = Cast<TESNPC>(apThis->baseForm);
    if (pNpc)
    {
        spdlog::info("Spawn Actor: {:X}, and NPC {}", apThis->formID, pNpc->fullName.value);
    }

    return ThisCall(RealSpawnActorInWorld, apThis);
}

TP_THIS_FUNCTION(TDamageActor, bool, Actor, float aDamage, Actor* apHitter, bool aKillMove);
static TDamageActor* RealDamageActor = nullptr;

// TODO: this is flawed, since it does not account for invulnerable actors
bool TP_MAKE_THISCALL(HookDamageActor, Actor, float aDamage, Actor* apHitter, bool aKillMove)
{
    float realDamage = GameplayFormulas::CalculateRealDamage(apThis, aDamage, aKillMove);

    float currentHealth = apThis->GetActorValue(ActorValueInfo::kHealth);
    bool wouldKill = (currentHealth - realDamage) <= 0.f;

    const auto* pExHittee = apThis->GetExtension();
    if (pExHittee->IsLocalPlayer())
    {
        if (!World::Get().GetServerSettings().PvpEnabled)
        {
            if (apHitter && apHitter->GetExtension()->IsRemotePlayer())
                return false;
        }

        World::Get().GetRunner().Trigger(HealthChangeEvent(apThis->formID, -realDamage));
        return ThisCall(RealDamageActor, apThis, aDamage, apHitter, aKillMove);
    }
    else if (pExHittee->IsRemotePlayer())
    {
        return wouldKill;
    }

    if (apHitter)
    {
        const auto* pExHitter = apHitter->GetExtension();
        if (pExHitter->IsLocalPlayer())
        {
            World::Get().GetRunner().Trigger(HealthChangeEvent(apThis->formID, -realDamage));
            return ThisCall(RealDamageActor, apThis, aDamage, apHitter, aKillMove);
        }
        if (pExHitter->IsRemotePlayer())
        {
            return wouldKill;
        }
    }

    if (pExHittee->IsLocal())
    {
        World::Get().GetRunner().Trigger(HealthChangeEvent(apThis->formID, -realDamage));
        return ThisCall(RealDamageActor, apThis, aDamage, apHitter, aKillMove);
    }
    else
    {
        return wouldKill;
    }
}

TP_THIS_FUNCTION(TApplyActorEffect, void, ActiveEffect, Actor* apTarget, float aEffectValue, unsigned int unk1);
static TApplyActorEffect* RealApplyActorEffect = nullptr;

void TP_MAKE_THISCALL(HookApplyActorEffect, ActiveEffect, Actor* apTarget, float aEffectValue, unsigned int unk1)
{
    const auto* pValueModEffect = Cast<ValueModifierEffect>(apThis);

    if (pValueModEffect)
    {
        if (pValueModEffect->actorValueIndex == ActorValueInfo::kHealth && aEffectValue > 0.0f)
        {
            if (apTarget && apTarget->GetExtension())
            {
                const auto pExTarget = apTarget->GetExtension();
                if (pExTarget->IsLocal())
                {
                    World::Get().GetRunner().Trigger(HealthChangeEvent(apTarget->formID, aEffectValue));
                    return ThisCall(RealApplyActorEffect, apThis, apTarget, aEffectValue, unk1);
                }
                return;
            }
        }
    }

    return ThisCall(RealApplyActorEffect, apThis, apTarget, aEffectValue, unk1);
}

TP_THIS_FUNCTION(TRegenAttributes, void*, Actor, int aId, float regenValue);
static TRegenAttributes* RealRegenAttributes = nullptr;

void* TP_MAKE_THISCALL(HookRegenAttributes, Actor, int aId, float aRegenValue)
{
    if (aId != ActorValueInfo::kHealth)
    {
        return ThisCall(RealRegenAttributes, apThis, aId, aRegenValue);
    }

    const auto* pExTarget = apThis->GetExtension();
    if (pExTarget->IsRemote())
    {
        return 0;
    }

    World::Get().GetRunner().Trigger(HealthChangeEvent(apThis->formID, aRegenValue));
    return ThisCall(RealRegenAttributes, apThis, aId, aRegenValue);
}

void TP_MAKE_THISCALL(HookAddInventoryItem, Actor, TESBoundObject* apItem, ExtraDataList* apExtraData, int32_t aCount, TESObjectREFR* apOldOwner)
{
    if (!ScopedInventoryOverride::IsOverriden())
    {
        auto& modSystem = World::Get().GetModSystem();

        Inventory::Entry item{};
        modSystem.GetServerModId(apItem->formID, item.BaseId);
        item.Count = aCount;

        if (apExtraData)
            apThis->GetItemFromExtraData(item, apExtraData);

        World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID, std::move(item)));
    }

    ThisCall(RealAddInventoryItem, apThis, apItem, apExtraData, aCount, apOldOwner);
}

void* TP_MAKE_THISCALL(HookPickUpObject, Actor, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, float aUnk2)
{
    if (!ScopedInventoryOverride::IsOverriden())
    {
        // This is here so that objects that are picked up on both clients, aka non temps, are synced through activation sync
        if (apObject->IsTemporary() && !ScopedActivateOverride::IsOverriden())
        {
            auto& modSystem = World::Get().GetModSystem();

            Inventory::Entry item{};
            modSystem.GetServerModId(apObject->baseForm->formID, item.BaseId);
            item.Count = aCount;

            if (apObject->GetExtraDataList())
                apThis->GetItemFromExtraData(item, apObject->GetExtraDataList());

            World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID, std::move(item)));
        }
    }

    return ThisCall(RealPickUpObject, apThis, apObject, aCount, aUnk1, aUnk2);
}

void Actor::PickUpObject(TESObjectREFR* apObject, int32_t aCount, bool aUnk1, float aUnk2) noexcept
{
    ThisCall(RealPickUpObject, this, apObject, aCount, aUnk1, aUnk2);
}

void* TP_MAKE_THISCALL(HookDropObject, Actor, void* apResult, TESBoundObject* apObject, ExtraDataList* apExtraData, int32_t aCount, NiPoint3* apLocation, NiPoint3* apRotation)
{
    auto& modSystem = World::Get().GetModSystem();

    Inventory::Entry item{};
    modSystem.GetServerModId(apObject->formID, item.BaseId);
    item.Count = -aCount;

    if (apExtraData)
        apThis->GetItemFromExtraData(item, apExtraData);

    World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID, std::move(item), true));

    ScopedInventoryOverride _;

    return ThisCall(RealDropObject, apThis, apResult, apObject, apExtraData, aCount, apLocation, apRotation);
}

void Actor::DropOrPickUpObject(const Inventory::Entry& arEntry, NiPoint3* apLocation, NiPoint3* apRotation) noexcept
{
    ExtraDataList* pExtraData = GetExtraDataFromItem(arEntry);

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
        DropObject(pObject, pExtraData, -arEntry.Count, apLocation, apRotation);
    // TODO: pick up
}

void Actor::DropObject(TESBoundObject* apObject, ExtraDataList* apExtraData, int32_t aCount, NiPoint3* apLocation, NiPoint3* apRotation) noexcept
{
    spdlog::debug("Dropping object, form id: {:X}, count: {}, actor: {:X}", apObject->formID, aCount, formID);
    BSPointerHandle<TESObjectREFR> result{};
    ThisCall(RealDropObject, this, &result, apObject, apExtraData, aCount, apLocation, apRotation);
}

TP_THIS_FUNCTION(TUpdateDetectionState, void, ActorKnowledge, void*);
static TUpdateDetectionState* RealUpdateDetectionState = nullptr;

void TP_MAKE_THISCALL(HookUpdateDetectionState, ActorKnowledge, void* apState)
{
    auto pOwner = TESObjectREFR::GetByHandle(apThis->hOwner);
    auto pTarget = TESObjectREFR::GetByHandle(apThis->hTarget);

    if (pOwner && pTarget)
    {
        auto pOwnerActor = Cast<Actor>(pOwner);
        auto pTargetActor = Cast<Actor>(pTarget);
        if (pOwnerActor && pTargetActor)
        {
            if (pOwnerActor->GetExtension()->IsRemotePlayer() && pTargetActor->GetExtension()->IsLocalPlayer())
            {
                spdlog::debug("Cancelling detection from remote player to local player, owner: {:X}, target: {:X}", pOwner->formID, pTarget->formID);
                return;
            }
        }
    }

    return ThisCall(RealUpdateDetectionState, apThis, apState);
}

struct DialogueItem;

// TODO: This is an AIProcess function
TP_THIS_FUNCTION(TProcessResponse, uint64_t, void, DialogueItem* apVoice, Actor* apTalkingActor, Actor* apTalkedToActor);
static TProcessResponse* RealProcessResponse = nullptr;

uint64_t TP_MAKE_THISCALL(HookProcessResponse, void, DialogueItem* apVoice, Actor* apTalkingActor, Actor* apTalkedToActor)
{
    if (apTalkingActor)
    {
        if (apTalkingActor->GetExtension()->IsRemotePlayer())
            return 0;
    }
    return ThisCall(RealProcessResponse, apThis, apVoice, apTalkingActor, apTalkedToActor);
}

bool TP_MAKE_THISCALL(HookInitiateMountPackage, Actor, Actor* apMount)
{
    if (apMount && apThis->GetExtension()->IsLocal())
        World::Get().GetRunner().Trigger(MountEvent(apThis->formID, apMount->formID));

    return ThisCall(RealInitiateMountPackage, apThis, apMount);
}

TP_THIS_FUNCTION(TUnequipObject, void, Actor, void* apUnk1, TESBoundObject* apObject, int32_t aUnk2, void* apUnk3);
static TUnequipObject* RealUnequipObject = nullptr;

void TP_MAKE_THISCALL(HookUnequipObject, Actor, void* apUnk1, TESBoundObject* apObject, int32_t aUnk2, void* apUnk3)
{
    ThisCall(RealUnequipObject, apThis, apUnk1, apObject, aUnk2, apUnk3);
}

TP_THIS_FUNCTION(TSpeakSoundFunction, bool, Actor, const char* apName, uint32_t* a3, uint32_t a4, uint32_t a5, uint32_t a6, uint64_t a7, uint64_t a8, uint64_t a9, bool a10, uint64_t a11, bool a12, bool a13, bool a14);
static TSpeakSoundFunction* RealSpeakSoundFunction = nullptr;

bool TP_MAKE_THISCALL(HookSpeakSoundFunction, Actor, const char* apName, uint32_t* a3, uint32_t a4, uint32_t a5, uint32_t a6, uint64_t a7, uint64_t a8, uint64_t a9, bool a10, uint64_t a11, bool a12, bool a13, bool a14)
{
    spdlog::debug("a3: {:X}, a4: {}, a5: {}, a6: {}, a7: {}, a8: {:X}, a9: {:X}, a10: {}, a11: {:X}, a12: {}, a13: {}, a14: {}",
                  (uint64_t)a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14);

    if (apThis->GetExtension()->IsLocal())
        World::Get().GetRunner().Trigger(DialogueEvent(apThis->formID, apName));

    return ThisCall(RealSpeakSoundFunction, apThis, apName, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14);
}

void Actor::SpeakSound(const char* pFile)
{
    uint32_t handle[3]{};
    handle[0] = -1;
    ThisCall(RealSpeakSoundFunction, this, pFile, handle, 0, 0x32, 0, 0, 0, 0, 0, 0, 0, 1, 1);
}

static TiltedPhoques::Initializer s_actorHooks([]()
{
    POINTER_SKYRIMSE(TCharacterConstructor, s_characterCtor, 40245);
    POINTER_SKYRIMSE(TCharacterConstructor2, s_characterCtor2, 40246);
    POINTER_SKYRIMSE(TCharacterDestructor, s_characterDtor, 37175);
    POINTER_SKYRIMSE(TGetLocation, s_GetActorLocation, 19812);
    POINTER_SKYRIMSE(TForceState, s_ForceState, 37313);
    POINTER_SKYRIMSE(TSpawnActorInWorld, s_SpawnActorInWorld, 19742);
    POINTER_SKYRIMSE(TDamageActor, s_damageActor, 37335);
    POINTER_SKYRIMSE(TApplyActorEffect, s_applyActorEffect, 35086);
    POINTER_SKYRIMSE(TRegenAttributes, s_regenAttributes, 37448);
    POINTER_SKYRIMSE(TAddInventoryItem, s_addInventoryItem, 37525);
    POINTER_SKYRIMSE(TPickUpObject, s_pickUpObject, 37521);
    POINTER_SKYRIMSE(TDropObject, s_dropObject, 40454);
    POINTER_SKYRIMSE(TUpdateDetectionState, s_updateDetectionState, 42704);
    POINTER_SKYRIMSE(TProcessResponse, s_processResponse, 39643);
    POINTER_SKYRIMSE(TInitiateMountPackage, s_initiateMountPackage, 37905);
    POINTER_SKYRIMSE(TUnequipObject, s_unequipObject, 37975);
    POINTER_SKYRIMSE(TSpeakSoundFunction, s_speakSoundFunction, 37542);

    FUNC_GetActorLocation = s_GetActorLocation.Get();
    RealCharacterConstructor = s_characterCtor.Get();
    RealCharacterConstructor2 = s_characterCtor2.Get();
    RealForceState = s_ForceState.Get();
    RealSpawnActorInWorld = s_SpawnActorInWorld.Get();
    RealDamageActor = s_damageActor.Get();
    RealApplyActorEffect = s_applyActorEffect.Get();
    RealRegenAttributes = s_regenAttributes.Get();
    RealAddInventoryItem = s_addInventoryItem.Get();
    RealPickUpObject = s_pickUpObject.Get();
    RealDropObject = s_dropObject.Get();
    RealUpdateDetectionState = s_updateDetectionState.Get();
    RealProcessResponse = s_processResponse.Get();
    RealInitiateMountPackage = s_initiateMountPackage.Get();
    RealUnequipObject = s_unequipObject.Get();
    RealSpeakSoundFunction = s_speakSoundFunction.Get();

    TP_HOOK(&RealCharacterConstructor, HookCharacterConstructor);
    TP_HOOK(&RealCharacterConstructor2, HookCharacterConstructor2);
    TP_HOOK(&RealForceState, HookForceState);
    TP_HOOK(&RealSpawnActorInWorld, HookSpawnActorInWorld);
    TP_HOOK(&RealDamageActor, HookDamageActor);
    TP_HOOK(&RealApplyActorEffect, HookApplyActorEffect);
    TP_HOOK(&RealRegenAttributes, HookRegenAttributes);
    TP_HOOK(&RealAddInventoryItem, HookAddInventoryItem);
    TP_HOOK(&RealPickUpObject, HookPickUpObject);
    TP_HOOK(&RealDropObject, HookDropObject);
    TP_HOOK(&RealUpdateDetectionState, HookUpdateDetectionState);
    TP_HOOK(&RealProcessResponse, HookProcessResponse);
    TP_HOOK(&RealInitiateMountPackage, HookInitiateMountPackage);
    TP_HOOK(&RealUnequipObject, HookUnequipObject);
    TP_HOOK(&RealSpeakSoundFunction, HookSpeakSoundFunction);
});
