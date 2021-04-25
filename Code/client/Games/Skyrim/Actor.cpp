#include <TiltedOnlinePCH.h>
#include <Games/References.h>
#include <Games/Skyrim/EquipManager.h>
#include <Misc/ActorProcessManager.h>
#include <Misc/MiddleProcess.h>
#include <Misc/GameVM.h>
#include <DefaultObjectManager.h>
#include <Forms/TESNPC.h>
#include <Forms/TESFaction.h>
#include <Components/TESActorBaseData.h>
#include <ExtraData/ExtraFactionChanges.h>
#include <Games/Memory.h>

#include <Events/HealthChangeEvent.h>

#include <World.h>
#include <Services/PapyrusService.h>

#include <Effects/ValueModifierEffect.h>
#include <Forms/ActorValueInfo.h>

#ifdef SAVE_STUFF

#include <Games/Skyrim/SaveLoad.h>

void Actor::Save_Reversed(const uint32_t aChangeFlags, Buffer::Writer& aWriter)
{
    BGSSaveFormBuffer buffer;

    Save(&buffer);

    ActorProcessManager* pProcessManager = processManager;
    const int32_t handlerId = pProcessManager != nullptr ? pProcessManager->handlerId : -1;

    aWriter.WriteBytes((uint8_t*)&handlerId, 4); // TODO: is this needed ?
    aWriter.WriteBytes((uint8_t*)&flags1, 4);

    //     if (!handlerId
//         && (uint8_t)ActorProcessManager::GetBoolInSubStructure(pProcessManager))
//     {
//         Actor::SaveSkinFar(this);
//     }


    TESObjectREFR::Save_Reversed(aChangeFlags, aWriter);

    if (pProcessManager); // Skyrim saves the process manager state, but we don't give a shit so skip !

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

using TGetLocation = TESForm *(TESForm *);
static TGetLocation *FUNC_GetActorLocation;

TCharacterConstructor* RealCharacterConstructor;
TCharacterConstructor2* RealCharacterConstructor2;
TCharacterDestructor* RealCharacterDestructor;

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


TESForm* Actor::GetEquippedWeapon(uint32_t aSlotId) const noexcept
{
    if (processManager && processManager->middleProcess)
    {
        auto pMiddleProcess = processManager->middleProcess;

        if (aSlotId == 0 && pMiddleProcess->leftEquippedObject)
            return *(pMiddleProcess->leftEquippedObject);

        else if (aSlotId == 1 && pMiddleProcess->rightEquippedObject)
            return *(pMiddleProcess->rightEquippedObject);

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

    return inventory;
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

    POINTER_SKYRIMSE(TSetFactionRankInternal, s_setFactionRankInternal, 0x1405F7AB0 - 0x140000000);

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
    if (equippedShout && (equippedShout->formType - 41) <= 1)
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
    if (!apHitter && pExHittee->IsLocal())
    {
        World::Get().GetRunner().Trigger(HealthChangeEvent(apThis, -aDamage));
        return ThisCall(RealDamageActor, apThis, aDamage, apHitter);
    }

    auto factions = apThis->GetFactions();
    for (const auto& faction : factions.NpcFactions)
    {
        if (faction.Id.BaseId == 0x00000DB1 && pExHittee->IsRemote())
        {
            return 0;
        }
        else if (faction.Id.BaseId == 0x00000DB1 && pExHittee->IsLocal())
        {
            World::Get().GetRunner().Trigger(HealthChangeEvent(apThis, -aDamage));
            return ThisCall(RealDamageActor, apThis, aDamage, apHitter);
        }
    }

    const auto pExHitter = apHitter->GetExtension();
    if (pExHitter->IsLocal())
    {
        World::Get().GetRunner().Trigger(HealthChangeEvent(apThis, -aDamage));
        return ThisCall(RealDamageActor, apThis, aDamage, apHitter);
    }

    return 0;
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
                World::Get().GetRunner().Trigger(HealthChangeEvent(apTarget, aEffectValue));
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

    World::Get().GetRunner().Trigger(HealthChangeEvent(apThis, aRegenValue));
    return ThisCall(RealRegenAttributes, apThis, aId, aRegenValue);
}

static TiltedPhoques::Initializer s_actorHooks([]()
    {
        POINTER_SKYRIMSE(TCharacterConstructor, s_characterCtor, 0x1406928C0 - 0x140000000);
        POINTER_SKYRIMSE(TCharacterConstructor2, s_characterCtor2, 0x1406929C0 - 0x140000000);
        POINTER_SKYRIMSE(TCharacterDestructor, s_characterDtor, 0x1405CDDA0 - 0x140000000);
        POINTER_SKYRIMSE(TGetLocation, s_GetActorLocation, 0x1402994F0 - 0x140000000);
        POINTER_SKYRIMSE(TForceState, s_ForceState, 0x1405D4090 - 0x140000000);
        POINTER_SKYRIMSE(TSpawnActorInWorld, s_SpawnActorInWorld, 0x140294000 - 0x140000000);
        POINTER_SKYRIMSE(TDamageActor, s_damageActor, 0x1405D6300 - 0x140000000);
        POINTER_SKYRIMSE(TApplyActorEffect, s_applyActorEffect, 0x140567A89 - 0x140000000);
        POINTER_SKYRIMSE(TRegenAttributes, s_regenAttributes, 0x140620900 - 0x140000000);

        FUNC_GetActorLocation = s_GetActorLocation.Get();
        RealCharacterConstructor = s_characterCtor.Get();
        RealCharacterConstructor2 = s_characterCtor2.Get();
        RealForceState = s_ForceState.Get();
        RealSpawnActorInWorld = s_SpawnActorInWorld.Get();
        RealDamageActor = s_damageActor.Get();
        RealApplyActorEffect = s_applyActorEffect.Get();
        RealRegenAttributes = s_regenAttributes.Get();

        TP_HOOK(&RealCharacterConstructor, HookCharacterConstructor);
        TP_HOOK(&RealCharacterConstructor2, HookCharacterConstructor2);
        TP_HOOK(&RealForceState, HookForceState);
        TP_HOOK(&RealSpawnActorInWorld, HookSpawnActorInWorld);
        TP_HOOK(&RealDamageActor, HookDamageActor);
        TP_HOOK(&RealApplyActorEffect, HookApplyActorEffect);
        TP_HOOK(&RealRegenAttributes, HookRegenAttributes);

    });
