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

#include <World.h>
#include <Services/PapyrusService.h>

#include <Forms/ActorValueInfo.h>

#include <Effects/ValueModifierEffect.h>

#include <Games/Skyrim/Misc/InventoryEntry.h>
#include <Games/Skyrim/ExtraData/ExtraCount.h>
#include <Games/Misc/ActorKnowledge.h>

#include <ExtraData/ExtraCharge.h>
#include <ExtraData/ExtraCount.h>
#include <ExtraData/ExtraEnchantment.h>
#include <ExtraData/ExtraHealth.h>
#include <ExtraData/ExtraPoison.h>
#include <ExtraData/ExtraSoul.h>
#include <ExtraData/ExtraTextDisplayData.h>
#include <Forms/EnchantmentItem.h>
#include <Forms/AlchemyItem.h>

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
TP_THIS_FUNCTION(TAddInventoryItem, void, Actor, TESBoundObject* apItem, BSExtraDataList* apExtraData, uint32_t aCount, TESObjectREFR* apOldOwner);
TP_THIS_FUNCTION(TPickUpItem, void*, Actor, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, float aUnk2);

using TGetLocation = TESForm *(TESForm *);
static TGetLocation *FUNC_GetActorLocation;

TCharacterConstructor* RealCharacterConstructor;
TCharacterConstructor2* RealCharacterConstructor2;
TCharacterDestructor* RealCharacterDestructor;

static TAddInventoryItem* RealAddInventoryItem = nullptr;
static TPickUpItem* RealPickUpItem = nullptr;

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

    POINTER_SKYRIMSE(TInterruptCast, s_interruptCast, 0x140657830 - 0x140000000);

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

Inventory Actor::GetInventory() const noexcept
{
    auto& modSystem = World::Get().GetModSystem();

    Inventory inventory;
    inventory.Buffer = SerializeInventory();

    auto pMainHandWeapon = GetEquippedWeapon(0);
    uint32_t mainId = pMainHandWeapon ? pMainHandWeapon->formID : 0;
    modSystem.GetServerModId(mainId, inventory.LeftHandWeapon);

    auto pSecondaryHandWeapon = GetEquippedWeapon(1);
    uint32_t secondaryId = pSecondaryHandWeapon ? pSecondaryHandWeapon->formID : 0;
    modSystem.GetServerModId(secondaryId, inventory.RightHandWeapon);

    mainId = magicItems[0] ? magicItems[0]->formID : 0;
    modSystem.GetServerModId(mainId, inventory.LeftHandSpell);

    secondaryId = magicItems[1] ? magicItems[1]->formID : 0;
    modSystem.GetServerModId(secondaryId, inventory.RightHandSpell);

    uint32_t shoutId = equippedShout ? equippedShout->formID : 0;
    modSystem.GetServerModId(shoutId, inventory.Shout);

    auto pAmmo = GetEquippedAmmo();
    uint32_t ammoId = pAmmo ? pAmmo->formID : 0;
    modSystem.GetServerModId(ammoId, inventory.Ammo);

    inventory.IsWeaponDrawn = actorState.IsWeaponDrawn();

    return inventory;
}

Container Actor::GetFullContainer() const noexcept
{
    auto& modSystem = World::Get().GetModSystem();
    Container fullContainer{};

    if (TESContainer* pBaseContainer = GetContainer())
    {
        for (int i = 0; i < pBaseContainer->count; i++)
        {
            TESContainer::Entry* pGameEntry = pBaseContainer->entries[i];
            if (!pGameEntry || !pGameEntry->form)
            {
                spdlog::warn("Entry or form for inventory item is null.");
                continue;
            }

            Container::Entry entry;
            modSystem.GetServerModId(pGameEntry->form->formID, entry.BaseId);
            entry.Count = pGameEntry->count;

            fullContainer.Entries.push_back(std::move(entry));
        }
    }

    Container extraContainer{};

    auto pExtraContChangesEntries = GetContainerChanges()->entries;
    for (auto pGameEntry : *pExtraContChangesEntries)
    {
        Container::Entry entry;
        modSystem.GetServerModId(pGameEntry->form->formID, entry.BaseId);
        entry.Count = pGameEntry->count;

        for (BSExtraDataList* pExtraDataList : *pGameEntry->dataList)
        {
            if (!pExtraDataList)
            {
                spdlog::warn("Null ExtraDataList?");
                continue;
            }

            Container::Entry innerEntry;
            innerEntry.BaseId = entry.BaseId;
            innerEntry.Count = 1;

            if (ExtraCount* pExtraCount = (ExtraCount*)pExtraDataList->GetByType(ExtraData::Count))
            {
                innerEntry.Count = pExtraCount->count;
            }

            if (ExtraCharge* pExtraCharge = (ExtraCharge*)pExtraDataList->GetByType(ExtraData::Charge))
            {
                innerEntry.ExtraCharge = pExtraCharge->fCharge;
            }

            if (ExtraEnchantment* pExtraEnchantment = (ExtraEnchantment*)pExtraDataList->GetByType(ExtraData::Enchantment))
            {
                TP_ASSERT(pExtraEnchantment->pEnchantment, "Null enchantment in ExtraEnchantment");
                // TODO: enchantments seem to always be temporaries, keep this in mind when trying to apply container
                // Get base form id of enchantment instead? Probably gonna have to serialize more data
                modSystem.GetServerModId(pExtraEnchantment->pEnchantment->formID, innerEntry.ExtraEnchantId);
                innerEntry.ExtraEnchantCharge = pExtraEnchantment->usCharge;
                innerEntry.ExtraEnchantRemoveUnequip = pExtraEnchantment->bRemoveOnUnequip;
            }

            if (ExtraHealth* pExtraHealth = (ExtraHealth*)pExtraDataList->GetByType(ExtraData::Health))
            {
                innerEntry.ExtraHealth = pExtraHealth->fHealth;
            }

            if (ExtraPoison* pExtraPoison = (ExtraPoison*)pExtraDataList->GetByType(ExtraData::Poison))
            {
                TP_ASSERT(pExtraPoison->pPoison, "Null poison in ExtraPoison");
                modSystem.GetServerModId(pExtraPoison->pPoison->formID, innerEntry.ExtraPoisonId);
                innerEntry.ExtraPoisonCount = pExtraPoison->uiCount;
            }

            if (ExtraSoul* pExtraSoul = (ExtraSoul*)pExtraDataList->GetByType(ExtraData::Soul))
            {
                innerEntry.ExtraSoulLevel = (int32_t)pExtraSoul->cSoul;
            }

            if (ExtraTextDisplayData* pExtraTextDisplayData = (ExtraTextDisplayData*)pExtraDataList->GetByType(ExtraData::TextDisplayData))
            {
                if (pExtraTextDisplayData->DisplayName)
                    innerEntry.ExtraTextDisplayName = pExtraTextDisplayData->DisplayName;
                else
                    innerEntry.ExtraTextDisplayName = "NULL DISPLAY NAME";
            }

            innerEntry.ExtraWorn = pExtraDataList->Contains(ExtraData::Worn);
            innerEntry.ExtraWornLeft = pExtraDataList->Contains(ExtraData::WornLeft);

            entry.Count -= innerEntry.Count;

            extraContainer.Entries.push_back(std::move(innerEntry));
        }

        if (entry.Count != 0)
            extraContainer.Entries.push_back(std::move(entry));
    }

    spdlog::info("ExtraContainer count: {}", extraContainer.Entries.size());

    Container minimizedExtraContainer{};

    for (auto& entry : extraContainer.Entries)
    {
        auto& duplicate = std::find_if(minimizedExtraContainer.Entries.begin(), minimizedExtraContainer.Entries.end(), [entry](Container::Entry newEntry) { 
            return newEntry.CanBeMerged(entry);
        });

        if (duplicate == std::end(minimizedExtraContainer.Entries))
        {
            minimizedExtraContainer.Entries.push_back(entry);
            continue;
        }

        duplicate->Count += entry.Count;
    }

    spdlog::info("MinExtraContainer count: {}", minimizedExtraContainer.Entries.size());

    for (auto& entry : minimizedExtraContainer.Entries)
    {
        if (entry.ContainsExtraData())
            continue;

        auto& duplicate = std::find_if(fullContainer.Entries.begin(), fullContainer.Entries.end(), [entry](Container::Entry newEntry) { 
            return newEntry.CanBeMerged(entry);
        });

        if (duplicate == std::end(fullContainer.Entries))
            continue;

        entry.Count += duplicate->Count;
        duplicate->Count = 0;
    }

    spdlog::info("MinExtraContainer count after: {}", minimizedExtraContainer.Entries.size());

    fullContainer.Entries.insert(fullContainer.Entries.end(), minimizedExtraContainer.Entries.begin(),
                                 minimizedExtraContainer.Entries.end());

    std::remove_if(fullContainer.Entries.begin(), fullContainer.Entries.end(), [](Container::Entry entry) { 
        return entry.Count == 0;
    });

    spdlog::info("fullContainer count after: {}", fullContainer.Entries.size());

    // TODO: doesn't filter all duplicates?
    return fullContainer;
}

Factions Actor::GetFactions() const noexcept
{
    Factions result;

    auto& modSystem = World::Get().GetModSystem();

    auto* pNpc = RTTI_CAST(baseForm, TESForm, TESNPC);
    if (pNpc)
    {
        auto& factions = pNpc->actorData.factions;

        for (auto i = 0; i < factions.length; ++i)
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
        for (auto i = 0; i < pChanges->entries.length; ++i)
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

void Actor::SetInventory(const Inventory& acInventory) noexcept
{
    UnEquipAll();

    auto* pEquipManager = EquipManager::Get();

    if (!acInventory.Buffer.empty())
        DeserializeInventory(acInventory.Buffer);

    auto& modSystem = World::Get().GetModSystem();

    uint32_t mainHandWeaponId = modSystem.GetGameId(acInventory.LeftHandWeapon);

    if (mainHandWeaponId)
        pEquipManager->Equip(this, TESForm::GetById(mainHandWeaponId), nullptr, 1, DefaultObjectManager::Get().leftEquipSlot, false, true, false, false);

    uint32_t secondaryHandWeaponId = modSystem.GetGameId(acInventory.RightHandWeapon);

    if (secondaryHandWeaponId)
        pEquipManager->Equip(this, TESForm::GetById(secondaryHandWeaponId), nullptr, 1, DefaultObjectManager::Get().rightEquipSlot, false, true, false, false);

    mainHandWeaponId = modSystem.GetGameId(acInventory.LeftHandSpell);

    if (mainHandWeaponId)
        pEquipManager->EquipSpell(this, TESForm::GetById(mainHandWeaponId), 0);

    secondaryHandWeaponId = modSystem.GetGameId(acInventory.RightHandSpell);

    if (secondaryHandWeaponId)
        pEquipManager->EquipSpell(this, TESForm::GetById(secondaryHandWeaponId), 1);

    uint32_t shoutId = modSystem.GetGameId(acInventory.Shout);

    if (shoutId)
        pEquipManager->EquipShout(this, TESForm::GetById(shoutId));

    uint32_t ammoId = modSystem.GetGameId(acInventory.Ammo);

    if (ammoId)
    {
        auto* pAmmo = TESForm::GetById(ammoId);

        auto count = GetItemCountInInventory(pAmmo);

        const auto pContainerChanges = GetContainerChanges()->entries;
        for (auto pChange : *pContainerChanges)
        {
            if (pChange && pChange->form && pChange->form->formID == ammoId)
            {
                if (pChange->form->formID != ammoId)
                    continue;

                const auto pDataLists = pChange->dataList;
                for (auto* pDataList : *pDataLists)
                {
                    if (pDataList)
                    {
                        if (pDataList->Contains(ExtraData::Count))
                        {
                            BSExtraData* pExtraData = pDataList->GetByType(ExtraData::Count);
                            ExtraCount* pExtraCount = RTTI_CAST(pExtraData, BSExtraData, ExtraCount);
                            if (pExtraCount)
                            {
                                pExtraCount->count = 0;
                            }
                        }
                    }
                }
            }
        }

        pEquipManager->Equip(this, pAmmo, nullptr, count, DefaultObjectManager::Get().rightEquipSlot, false, true, false, false);
    }

    // TODO: check if weapon drawn state is the same
    SetWeaponDrawnEx(acInventory.IsWeaponDrawn);
}

void Actor::ForceActorValue(uint32_t aMode, uint32_t aId, float aValue) noexcept
{
    const float current = GetActorValue(aId);
    actorValueOwner.ForceCurrent(aMode, aId, aValue - current);
}

void Actor::SetActorValue(uint32_t aId, float aValue) noexcept
{
    actorValueOwner.SetValue(aId, aValue);
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

    POINTER_SKYRIMSE(TSetFactionRankInternal, s_setFactionRankInternal, 0x14061E5B0 - 0x140000000);

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

    RemoveAllItems();

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
            const auto pExTarget = apTarget->GetExtension();
            if (pExTarget->IsLocal())
            {
                World::Get().GetRunner().Trigger(HealthChangeEvent(apTarget->formID, aEffectValue));
                return ThisCall(RealApplyActorEffect, apThis, apTarget, aEffectValue, unk1);
            }
            return;
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

void TP_MAKE_THISCALL(HookAddInventoryItem, Actor, TESBoundObject* apItem, BSExtraDataList* apExtraData, uint32_t aCount, TESObjectREFR* apOldOwner)
{
    World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID));
    ThisCall(RealAddInventoryItem, apThis, apItem, apExtraData, aCount, apOldOwner);
}

void* TP_MAKE_THISCALL(HookPickUpItem, Actor, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, float aUnk2)
{
    World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID));
    return ThisCall(RealPickUpItem, apThis, apObject, aCount, aUnk1, aUnk2);
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

// This is an AIProcess function
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

static TiltedPhoques::Initializer s_actorHooks([]()
{
    POINTER_SKYRIMSE(TCharacterConstructor, s_characterCtor, 0x1406BA280 - 0x140000000);
    POINTER_SKYRIMSE(TCharacterConstructor2, s_characterCtor2, 0x1406BA510 - 0x140000000);
    POINTER_SKYRIMSE(TCharacterDestructor, s_characterDtor, 0x1405F20A0 - 0x140000000);
    POINTER_SKYRIMSE(TGetLocation, s_GetActorLocation, 0x1402ABAB0 - 0x140000000);
    POINTER_SKYRIMSE(TForceState, s_ForceState, 0x1405F85D0 - 0x140000000);
    POINTER_SKYRIMSE(TSpawnActorInWorld, s_SpawnActorInWorld, 0x1402A6610 - 0x140000000);
    POINTER_SKYRIMSE(TDamageActor, s_damageActor, 0x1405FA9A0 - 0x140000000);
    POINTER_SKYRIMSE(TApplyActorEffect, s_applyActorEffect, 0x140584369 - 0x140000000);
    POINTER_SKYRIMSE(TRegenAttributes, s_regenAttributes, 0x140606DF0 - 0x140000000);
    POINTER_SKYRIMSE(TAddInventoryItem, s_addInventoryItem, 0x14060CC10 - 0x140000000);
    POINTER_SKYRIMSE(TPickUpItem, s_pickUpItem, 0x14060C280 - 0x140000000);
    POINTER_SKYRIMSE(TUpdateDetectionState, s_updateDetectionState, 0x140742FE0 - 0x140000000);
    POINTER_SKYRIMSE(TProcessResponse, s_processResponse, 0x14068BC50 - 0x140000000);

    FUNC_GetActorLocation = s_GetActorLocation.Get();
    RealCharacterConstructor = s_characterCtor.Get();
    RealCharacterConstructor2 = s_characterCtor2.Get();
    RealForceState = s_ForceState.Get();
    RealSpawnActorInWorld = s_SpawnActorInWorld.Get();
    RealDamageActor = s_damageActor.Get();
    RealApplyActorEffect = s_applyActorEffect.Get();
    RealRegenAttributes = s_regenAttributes.Get();
    RealAddInventoryItem = s_addInventoryItem.Get();
    RealPickUpItem = s_pickUpItem.Get();
    RealUpdateDetectionState = s_updateDetectionState.Get();
    RealProcessResponse = s_processResponse.Get();

    TP_HOOK(&RealCharacterConstructor, HookCharacterConstructor);
    TP_HOOK(&RealCharacterConstructor2, HookCharacterConstructor2);
    TP_HOOK(&RealForceState, HookForceState);
    TP_HOOK(&RealSpawnActorInWorld, HookSpawnActorInWorld);
    TP_HOOK(&RealDamageActor, HookDamageActor);
    TP_HOOK(&RealApplyActorEffect, HookApplyActorEffect);
    TP_HOOK(&RealRegenAttributes, HookRegenAttributes);
    TP_HOOK(&RealAddInventoryItem, HookAddInventoryItem);
    TP_HOOK(&RealPickUpItem, HookPickUpItem);
    TP_HOOK(&RealUpdateDetectionState, HookUpdateDetectionState);
    TP_HOOK(&RealProcessResponse, HookProcessResponse);
});
