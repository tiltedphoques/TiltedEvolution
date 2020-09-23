#if TP_FALLOUT4

#include <Games/Fallout4/PlayerCharacter.h>
#include <Games/Fallout4/Forms/TESFaction.h>
#include <Games/Fallout4/Forms/TESNPC.h>
#include <Games/Fallout4/ExtraData/ExtraFactionChanges.h>
#include <Games/Memory.h>
#include <Games/Overrides.h>

#include <World.h>
#include <Services/PapyrusService.h>

TP_THIS_FUNCTION(TActorConstructor, Actor*, Actor, uint8_t aUnk);
TP_THIS_FUNCTION(TActorConstructor2, Actor*, Actor, volatile int** aRefCount, uint8_t aUnk);

static TActorConstructor* RealActorConstructor;
static TActorConstructor2* RealActorConstructor2;

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

GamePtr<Actor> Actor::New() noexcept
{
    const auto pActor = Memory::Allocate<Actor>();

    ThisCall(HookActorContructor, pActor, uint8_t(1));

    return pActor;
}

TESForm* Actor::GetEquippedWeapon(uint32_t aSlotId) const noexcept
{
    using TGetEquippedWeapon = TESForm* (__fastcall)(void*, void*, const Actor*, uint32_t);
    
    POINTER_FALLOUT4(TGetEquippedWeapon, s_getEquippedWeapon, 0x141388BC0 - 0x140000000);

    return s_getEquippedWeapon(nullptr, nullptr, this, aSlotId);
}

Inventory Actor::GetInventory() const noexcept
{
    auto& modSystem = World::Get().GetModSystem();

    Inventory InvetorySave;
    InvetorySave.Buffer = SerializeInventory();

    auto pMainHandWeapon = GetEquippedWeapon(0);
    uint32_t mainId = pMainHandWeapon ? pMainHandWeapon->formID : 0;
    modSystem.GetServerModId(mainId, InvetorySave.RightHandWeapon);

    return InvetorySave;
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

    auto pFactionExtras = RTTI_CAST(extraData->GetByType(ExtraData::Faction), BSExtraData, ExtraFactionChanges);
    if (pFactionExtras)
    {
        for (auto i = 0; i < pFactionExtras->entries.length; ++i)
        {
            Faction faction;

            modSystem.GetServerModId(pFactionExtras->entries[i].faction->formID, faction.Id);
            faction.Rank = pFactionExtras->entries[i].rank;

            result.ExtraFactions.push_back(faction);
        }
    }

    return result;
}

void Actor::SetInventory(const Inventory& acInventory) noexcept
{
    UnEquipAll();

    if(!acInventory.Buffer.empty())
        DeserializeInventory(acInventory.Buffer);
}

void Actor::SetFactions(const Factions& acFactions) noexcept
{
    RemoveFromAllFactions();

    auto& modSystem = World::Get().GetModSystem();

    for (auto& entry : acFactions.NpcFactions)
    {
        auto pForm = GetById(modSystem.GetGameId(entry.Id));
        auto pFaction = RTTI_CAST(pForm, TESForm, TESFaction);
        if (pFaction)
        {
            SetFactionRank(pFaction, entry.Rank);
        }
    }

    for (auto& entry : acFactions.ExtraFactions)
    {
        auto pForm = GetById(modSystem.GetGameId(entry.Id));
        auto pFaction = RTTI_CAST(pForm, TESForm, TESFaction);
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

void Actor::UnEquipAll() noexcept
{
    TP_THIS_FUNCTION(TUnEquipAll, void, Actor);

    POINTER_FALLOUT4(TUnEquipAll, s_unequipAll, 0x140D8E370 - 0x140000000);

    ThisCall(s_unequipAll, this);
}

void Actor::RemoveFromAllFactions() noexcept
{
    PAPYRUS_FUNCTION(void, Actor, RemoveFromAllFactions);

    s_pRemoveFromAllFactions(this);
}

static TiltedPhoques::Initializer s_specificReferencesHooks([]()
    {
        POINTER_FALLOUT4(TActorConstructor, s_actorCtor, 0x140D6E9A0 - 0x140000000);
        POINTER_FALLOUT4(TActorConstructor2, s_actorCtor2, 0x140D6ED80 - 0x140000000);

        RealActorConstructor = s_actorCtor.Get();
        RealActorConstructor2 = s_actorCtor2.Get();

        TP_HOOK(&RealActorConstructor, HookActorContructor);
        TP_HOOK(&RealActorConstructor2, HookActorContructor2);
    });

#endif
