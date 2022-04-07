#include <TiltedOnlinePCH.h>
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

#include <World.h>
#include <Services/PapyrusService.h>

#include <Forms/ActorValueInfo.h>

#include <Effects/ValueModifierEffect.h>

#include <Games/Skyrim/Misc/InventoryEntry.h>
#include <Games/Skyrim/ExtraData/ExtraCount.h>
#include <Games/Misc/ActorKnowledge.h>

#include <Games/ExtraDataList.h>
#include <ExtraData/ExtraCharge.h>
#include <ExtraData/ExtraCount.h>
#include <ExtraData/ExtraEnchantment.h>
#include <ExtraData/ExtraHealth.h>
#include <ExtraData/ExtraPoison.h>
#include <ExtraData/ExtraSoul.h>
#include <ExtraData/ExtraTextDisplayData.h>
#include <Forms/EnchantmentItem.h>
#include <Forms/AlchemyItem.h>

#include <Games/Overrides.h>

#ifdef SAVE_STUFF

#include <Games/Skyrim/SaveLoad.h>

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

    auto* pNpc = RTTI_CAST(baseForm, TESForm, TESNPC);
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

    auto* pChanges = RTTI_CAST(extraData.GetByType(ExtraData::Faction), BSExtraData, ExtraFactionChanges);
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
        float maxValue = actorValueOwner.GetMaxValue(i);
        actorValues.ActorMaxValuesList.insert({i, maxValue});
    }

    return actorValues;
}

float Actor::GetActorValue(uint32_t aId) const noexcept
{
    return actorValueOwner.GetValue(aId);
}

float Actor::GetActorMaxValue(uint32_t aId) const noexcept
{
    return actorValueOwner.GetMaxValue(aId);
}

void Actor::SetActorValue(uint32_t aId, float aValue) noexcept
{
    actorValueOwner.SetValue(aId, aValue);
}

void Actor::ForceActorValue(uint32_t aMode, uint32_t aId, float aValue) noexcept
{
    const float current = GetActorValue(aId);
    actorValueOwner.ForceCurrent(aMode, aId, aValue - current);
}

Inventory Actor::GetActorInventory() const noexcept
{
    Inventory inventory = GetInventory();

    inventory.CurrentEquipment = GetEquipment();

    return inventory;
}

Equipment Actor::GetEquipment() const noexcept
{
    Equipment equipment;

    auto& modSystem = World::Get().GetModSystem();

    auto pMainHandWeapon = GetEquippedWeapon(0);
    uint32_t mainId = pMainHandWeapon ? pMainHandWeapon->formID : 0;
    modSystem.GetServerModId(mainId, equipment.LeftHandWeapon);

    auto pSecondaryHandWeapon = GetEquippedWeapon(1);
    uint32_t secondaryId = pSecondaryHandWeapon ? pSecondaryHandWeapon->formID : 0;
    modSystem.GetServerModId(secondaryId, equipment.RightHandWeapon);

    mainId = magicItems[0] ? magicItems[0]->formID : 0;
    modSystem.GetServerModId(mainId, equipment.LeftHandSpell);

    secondaryId = magicItems[1] ? magicItems[1]->formID : 0;
    modSystem.GetServerModId(secondaryId, equipment.RightHandSpell);

    uint32_t shoutId = equippedShout ? equippedShout->formID : 0;
    modSystem.GetServerModId(shoutId, equipment.Shout);

    auto pAmmo = GetEquippedAmmo();
    uint32_t ammoId = pAmmo ? pAmmo->formID : 0;
    modSystem.GetServerModId(ammoId, equipment.Ammo);

    return equipment;
}

void Actor::SetActorInventory(Inventory& aInventory) noexcept
{
    spdlog::info("Setting inventory for actor {:X}", formID);

    UnEquipAll();

    SetInventory(aInventory);
    SetEquipment(aInventory.CurrentEquipment);
}

// TODO: remove all the unequip stuff, not needed anymore
void Actor::SetEquipment(const Equipment& acEquipment) noexcept
{
    auto* pEquipManager = EquipManager::Get();
    auto& modSystem = World::Get().GetModSystem();

    const Equipment cCurrentEquipment = GetEquipment();

    if (acEquipment.LeftHandWeapon != cCurrentEquipment.LeftHandWeapon)
    {
        if (acEquipment.LeftHandWeapon)
        {
            uint32_t mainHandWeaponId = modSystem.GetGameId(acEquipment.LeftHandWeapon);
            pEquipManager->Equip(this, TESForm::GetById(mainHandWeaponId), nullptr, 1, DefaultObjectManager::Get().leftEquipSlot, false, true, false, false);
        }
        else
        {
            uint32_t mainHandWeaponId = modSystem.GetGameId(cCurrentEquipment.LeftHandWeapon);
            pEquipManager->UnEquip(this, TESForm::GetById(mainHandWeaponId), nullptr, 1, DefaultObjectManager::Get().leftEquipSlot, true, false, true, false, nullptr);
        }
    }

    if (acEquipment.RightHandWeapon != cCurrentEquipment.RightHandWeapon)
    {
        if (acEquipment.RightHandWeapon)
        {
            uint32_t secondaryHandWeaponId = modSystem.GetGameId(acEquipment.RightHandWeapon);
            pEquipManager->Equip(this, TESForm::GetById(secondaryHandWeaponId), nullptr, 1, DefaultObjectManager::Get().rightEquipSlot, false, true, false, false);
        }
        else
        {
            uint32_t secondaryHandWeaponId = modSystem.GetGameId(cCurrentEquipment.RightHandWeapon);
            pEquipManager->UnEquip(this, TESForm::GetById(secondaryHandWeaponId), nullptr, 1, DefaultObjectManager::Get().rightEquipSlot, true, false, true, false, nullptr);
        }
    }

    if (acEquipment.LeftHandSpell != cCurrentEquipment.LeftHandSpell)
    {
        if (acEquipment.LeftHandSpell)
        {
            uint32_t mainHandWeaponId = modSystem.GetGameId(acEquipment.LeftHandSpell);
            pEquipManager->EquipSpell(this, TESForm::GetById(mainHandWeaponId), 0);
        }
        else
        {
            uint32_t mainHandWeaponId = modSystem.GetGameId(cCurrentEquipment.LeftHandSpell);
            pEquipManager->UnEquipSpell(this, TESForm::GetById(mainHandWeaponId), 0);
        }
    }

    if (acEquipment.RightHandSpell != cCurrentEquipment.RightHandSpell)
    {
        if (acEquipment.RightHandSpell)
        {
            uint32_t secondaryHandWeaponId = modSystem.GetGameId(acEquipment.RightHandSpell);
            pEquipManager->EquipSpell(this, TESForm::GetById(secondaryHandWeaponId), 1);
        }
        else
        {
            uint32_t secondaryHandWeaponId = modSystem.GetGameId(cCurrentEquipment.RightHandSpell);
            pEquipManager->UnEquipSpell(this, TESForm::GetById(secondaryHandWeaponId), 1);
        }
    }

    if (acEquipment.Shout != cCurrentEquipment.Shout)
    {
        if (acEquipment.Shout)
        {
            uint32_t shoutId = modSystem.GetGameId(acEquipment.Shout);
            pEquipManager->EquipShout(this, TESForm::GetById(shoutId));
        }
        else
        {
            uint32_t shoutId = modSystem.GetGameId(cCurrentEquipment.Shout);
            pEquipManager->UnEquipShout(this, TESForm::GetById(shoutId));
        }
    }

    if (acEquipment.Ammo != cCurrentEquipment.Ammo)
    {
        if (acEquipment.Ammo)
        {
            uint32_t ammoId = modSystem.GetGameId(acEquipment.Ammo);
            TESForm* pAmmo = TESForm::GetById(ammoId);
            int64_t count = GetItemCountInInventory(pAmmo);
            pEquipManager->Equip(this, pAmmo, nullptr, count, DefaultObjectManager::Get().rightEquipSlot, false, true, false, false);
        }
        else
        {
            uint32_t ammoId = modSystem.GetGameId(cCurrentEquipment.Ammo);
            TESForm* pAmmo = TESForm::GetById(ammoId);
            int64_t count = GetItemCountInInventory(pAmmo);
            pEquipManager->UnEquip(this, pAmmo, nullptr, count, DefaultObjectManager::Get().rightEquipSlot, true, false, true, false, nullptr);
        }
    }
}

void Actor::SetActorValues(const ActorValues& acActorValues) noexcept
{
    for (auto& value : acActorValues.ActorMaxValuesList)
    {
        float current = actorValueOwner.GetValue(value.first);
        actorValueOwner.ForceCurrent(0, value.first, value.second - current);
    }

    for (auto& value : acActorValues.ActorValuesList)
    {
        float current = actorValueOwner.GetValue(value.first);
        actorValueOwner.ForceCurrent(2, value.first, value.second - current);
    }
}

void Actor::SetFactions(const Factions& acFactions) noexcept
{
    RemoveFromAllFactions();

    auto& modSystem = World::Get().GetModSystem();

    for (auto& entry : acFactions.NpcFactions)
    {
        auto pForm = TESForm::GetById(modSystem.GetGameId(entry.Id));
        auto pFaction = RTTI_CAST(pForm, TESForm, TESFaction);
        if (pFaction)
        {
            SetFactionRank(pFaction, entry.Rank);
        }
    }

    for (auto& entry : acFactions.ExtraFactions)
    {
        auto pForm = TESForm::GetById(modSystem.GetGameId(entry.Id));
        auto pFaction = RTTI_CAST(pForm, TESForm, TESFaction);
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
                    if (pDataList->Contains(ExtraData::Worn))
                    {
                        EquipManager::Get()->UnEquip(this, pChange->form, pDataList, 1, DefaultObjectManager::Get().rightEquipSlot, true, false, true, false, nullptr);
                    }

                    // Left slot
                    if (pDataList->Contains(ExtraData::WornLeft))
                    {
                        EquipManager::Get()->UnEquip(this, pChange->form, pDataList, 1, DefaultObjectManager::Get().leftEquipSlot, true, false, true, false, nullptr);
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
    if (!leftHandCaster)
    {
        MagicCaster* pCaster = GetMagicCaster(MagicSystem::CastingSource::LEFT_HAND);
        leftHandCaster = RTTI_CAST(pCaster, MagicCaster, ActorMagicCaster);
    }
    if (!rightHandCaster)
    {
        MagicCaster* pCaster = GetMagicCaster(MagicSystem::CastingSource::RIGHT_HAND);
        rightHandCaster = RTTI_CAST(pCaster, MagicCaster, ActorMagicCaster);
    }
    if (!shoutCaster)
    {
        MagicCaster* pCaster = GetMagicCaster(MagicSystem::CastingSource::OTHER);
        shoutCaster = RTTI_CAST(pCaster, MagicCaster, ActorMagicCaster);
    }
    if (!instantCaster)
    {
        MagicCaster* pCaster = GetMagicCaster(MagicSystem::CastingSource::INSTANT);
        instantCaster = RTTI_CAST(pCaster, MagicCaster, ActorMagicCaster);
    }
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

TP_THIS_FUNCTION(TForceState, void, Actor, const NiPoint3&, float, float, TESObjectCELL*, TESWorldSpace*, bool);
static TForceState* RealForceState = nullptr;

void TP_MAKE_THISCALL(HookForceState, Actor, const NiPoint3& acPosition, float aX, float aZ,
                      TESObjectCELL* apCell, TESWorldSpace* apWorldSpace, bool aUnkBool)
{
    /*const auto pNpc = RTTI_CAST(apThis->baseForm, TESForm, TESNPC);
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
    const auto* pNpc = RTTI_CAST(apThis->baseForm, TESForm, TESNPC);
    if (pNpc)
    {
        spdlog::info("Spawn Actor: {:X}, and NPC {}", apThis->formID, pNpc->fullName.value);
    }

    return ThisCall(RealSpawnActorInWorld, apThis);
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

TP_THIS_FUNCTION(TApplyActorEffect, void, ActiveEffect, Actor* apTarget, float aEffectValue, unsigned int unk1);
static TApplyActorEffect* RealApplyActorEffect = nullptr;

void TP_MAKE_THISCALL(HookApplyActorEffect, ActiveEffect, Actor* apTarget, float aEffectValue, unsigned int unk1)
{
    const auto* pValueModEffect = RTTI_CAST(apThis, ActiveEffect, ValueModifierEffect);

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
            
            // TODO: not sure about this
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

void Actor::DropObject(TESBoundObject* apObject, ExtraDataList* apExtraData, int32_t aCount, NiPoint3* apLocation, NiPoint3* apRotation) noexcept
{
    spdlog::debug("Dropping object, form id: {:X}, count: {}, actor: {:X}", apObject->formID, aCount, formID);
    BSPointerHandle<TESObjectREFR> result{};
    ThisCall(RealDropObject, this, &result, apObject, apExtraData, -aCount, apLocation, apRotation);
}

TP_THIS_FUNCTION(TUpdateDetectionState, void, ActorKnowledge, void*);
static TUpdateDetectionState* RealUpdateDetectionState = nullptr;

void TP_MAKE_THISCALL(HookUpdateDetectionState, ActorKnowledge, void* apState)
{
    auto pOwner = TESObjectREFR::GetByHandle(apThis->hOwner);
    auto pTarget = TESObjectREFR::GetByHandle(apThis->hTarget);

    if (pOwner && pTarget)
    {
        auto pOwnerActor = RTTI_CAST(pOwner, TESObjectREFR, Actor);
        auto pTargetActor = RTTI_CAST(pTarget, TESObjectREFR, Actor);
        if (pOwnerActor && pTargetActor)
        {
            if (pOwnerActor->GetExtension()->IsRemotePlayer() && pTargetActor->GetExtension()->IsLocalPlayer())
            {
                spdlog::info("Cancelling detection from remote player to local player, owner: {:X}, target: {:X}", pOwner->formID, pTarget->formID);
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
});
