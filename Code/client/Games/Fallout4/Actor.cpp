#include <TiltedOnlinePCH.h>

#include <Games/References.h>
#include <Games/Memory.h>
#include <Forms/ActorValueInfo.h>
#include <Forms/TESNPC.h>
#include <Games/TES.h>
#include <Games/Overrides.h>
#include <Forms/TESFaction.h>
#include <AI/AIProcess.h>
#include <Misc/MiddleProcess.h>
#include <ExtraData/ExtraFactionChanges.h>
#include <Magic/ActorMagicCaster.h>
#include <World.h>

ActorExtension* Actor::GetExtension() noexcept
{
    if (auto* pActor = AsExActor())
        return static_cast<ActorExtension*>(pActor);
    if (auto* pPlayer = AsExPlayerCharacter())
        return static_cast<ActorExtension*>(pPlayer);
    return nullptr;
}

ExActor* Actor::AsExActor() noexcept
{
    return formType == Type && this != PlayerCharacter::Get() ? static_cast<ExActor*>(this) : nullptr;
}

ExPlayerCharacter* Actor::AsExPlayerCharacter() noexcept
{
    return this == PlayerCharacter::Get() ? static_cast<ExPlayerCharacter*>(this) : nullptr;
}

GamePtr<Actor> Actor::New() noexcept
{
    auto* pActor = Memory::Allocate<Actor>();
    if (!pActor)
        return {};
    TP_THIS_FUNCTION(TConstructor, Actor*, Actor, bool);
    static VersionDbPtr<TConstructor> constructor(2229563);
    TiltedPhoques::ThisCall(constructor, pActor, false);
    return {pActor};
}

GamePtr<Actor> Actor::Create(TESNPC* apBaseForm) noexcept
{
    auto* pPlayer = PlayerCharacter::Get();
    auto* pManager = ModManager::Get();
    if (!apBaseForm || !pPlayer || !pManager)
        return {};

    auto pActor = New();
    if (!pActor)
        return {};
    pActor->SetSkipSaveFlag(true);
    pActor->GetExtension()->SetRemote(true);
    pActor->SetObjectReference(apBaseForm);
    pActor->SetParentCell(pPlayer->parentCell);

    auto position = pPlayer->position;
    auto rotation = pPlayer->rotation;
    if (!pManager->Spawn(position, rotation, pPlayer->parentCell, pPlayer->GetWorldSpace(), pActor))
        return {};
    return pActor;
}

GamePtr<Actor> Actor::Spawn(uint32_t aBaseFormId) noexcept
{
    return Create(Cast<TESNPC>(TESForm::GetById(aBaseFormId)));
}

uint16_t Actor::GetLevel() const noexcept
{
    TP_THIS_FUNCTION(TGetLevel, uint16_t, const Actor);
    static VersionDbPtr<TGetLevel> getLevel(2229734);
    return TiltedPhoques::ThisCall(getLevel, this);
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
    const float initialValue = aMode == ActorValueOwner::ForceMode::PERMANENT ? GetActorPermanentValue(aId) : GetActorValue(aId);
    if (aValue != initialValue)
        actorValueOwner.ForceCurrent(aMode, aId, aValue - initialValue);
}

ActorValues Actor::GetEssentialActorValues() const noexcept
{
    ActorValues values;
    const uint32_t essentialValues[] = {ActorValueInfo::kHealth, ActorValueInfo::kActionPoints, ActorValueInfo::kRads, ActorValueInfo::kRadHealthMax, ActorValueInfo::kFatigue, ActorValueInfo::kFatigueAPMax};
    for (const auto i : essentialValues)
    {
        if (!ActorValueInfo::Resolve(i))
            continue;
        values.ActorValuesList.emplace(i, GetActorValue(i));
        values.ActorMaxValuesList.emplace(i, GetActorPermanentValue(i));
    }
    return values;
}

void Actor::SetActorValues(const ActorValues& acActorValues) noexcept
{
    for (const auto& value : acActorValues.ActorMaxValuesList)
        ForceActorValue(ActorValueOwner::ForceMode::PERMANENT, value.first, value.second);
    for (const auto& value : acActorValues.ActorValuesList)
        ForceActorValue(ActorValueOwner::ForceMode::DAMAGE, value.first, value.second);
}

namespace
{
TP_THIS_FUNCTION(TDestructor, void, Actor);
TDestructor* s_destructor = nullptr;

void TP_MAKE_THISCALL(HookDestructor, Actor)
{
    if (auto* pExtension = apThis->GetExtension())
        pExtension->~ActorExtension();
    TiltedPhoques::ThisCall(s_destructor, apThis);
}

TiltedPhoques::Initializer s_actorHooks(
    []()
    {
        static VersionDbPtr<TDestructor> destructor(2229565);
        s_destructor = destructor.Get();
        TP_HOOK(&s_destructor, HookDestructor);
    });
}

bool Actor::IsDead() const noexcept
{
    // Papyrus Actor.IsDead calls the virtual with abNotEssential set.
    return static_cast<const TESObjectREFR*>(this)->IsDead(true);
}

bool Actor::IsDragon() const noexcept
{
    return false;
}

bool Actor::IsPlayerSummon() const noexcept
{
    const Actor* pCommandingActor = GetCommandingActor();
    return pCommandingActor && pCommandingActor->formID == 0x14;
}

Actor* Actor::GetCommandingActor() const noexcept
{
    if (currentProcess && currentProcess->middleProcess && currentProcess->middleProcess->commandingActor)
        return Cast<Actor>(TESObjectREFR::GetByHandle(currentProcess->middleProcess->commandingActor.handle.iBits));
    return nullptr;
}

void Actor::SetCommandingActor(BSPointerHandle<TESObjectREFR> aCommandingActor) noexcept
{
    if (currentProcess && currentProcess->middleProcess)
    {
        currentProcess->middleProcess->commandingActor = aCommandingActor;
        boolFlags |= ActorFlags::IS_COMMANDED_ACTOR;
    }
}

bool Actor::InitiateMountPackage(Actor*) noexcept
{
    // Fallout 4 has no rideable mounts.
    return false;
}

bool Actor::RemoveSpell(MagicItem* apSpell) noexcept
{
    if (!apSpell)
        return false;
    TP_THIS_FUNCTION(TRemoveSpell, bool, Actor, MagicItem*);
    static VersionDbPtr<TRemoveSpell> removeSpell(2231253);
    return TiltedPhoques::ThisCall(removeSpell, this, apSpell);
}

void Actor::ForcePosition(const NiPoint3& acPosition) noexcept
{
    ScopedReferencesOverride recursionGuard;
    SetPosition(acPosition, true);
}

void Actor::Kill() noexcept
{
    // Never kill players
    if (GetExtension()->IsPlayer())
        return;
    KillImpl(nullptr, 100.f, true, true);
}

void Actor::Respawn() noexcept
{
    Resurrect(false, true);
}

void Actor::SetPackage(TESPackage* apPackage) noexcept
{
    PutCreatedPackage(apPackage, false, true, true);
}

void Actor::SetEssentialEx(bool aSet) noexcept
{
    SetEssential(aSet);
    if (auto* pBase = Cast<TESNPC>(baseForm))
        pBase->actorData.SetEssential(aSet);
}

void Actor::SetNoBleedoutRecovery(bool aSet) noexcept
{
    TP_THIS_FUNCTION(TSetNoBleedoutRecovery, void, Actor, bool);
    static VersionDbPtr<TSetNoBleedoutRecovery> setNoBleedoutRecovery(2231052);
    TiltedPhoques::ThisCall(setNoBleedoutRecovery, this, aSet);
}

void Actor::SetFactionRank(const TESFaction* apFaction, int8_t aRank) noexcept
{
    TP_THIS_FUNCTION(TSetFactionRank, void, Actor, const TESFaction*, int8_t);
    static VersionDbPtr<TSetFactionRank> setFactionRank(2230101);
    TiltedPhoques::ThisCall(setFactionRank, this, apFaction, aRank);
}

void Actor::SetPlayerRespawnMode(bool aSet) noexcept
{
    SetEssentialEx(aSet);
    // Makes the player go in an unrecoverable bleedout state
    SetNoBleedoutRecovery(aSet);

    if (formID != 0x14)
    {
        // PlayerFaction
        if (auto* pPlayerFaction = Cast<TESFaction>(TESForm::GetById(0x1C21C)))
            SetFactionRank(pPlayerFaction, 1);
    }
}

void Actor::SetWeaponDrawnEx(bool aDraw) noexcept
{
    if (GetActorState()->IsWeaponDrawn() == aDraw)
        return;
    DrawWeaponMagicHands(aDraw);
}

void Actor::SpeakSound(const char*)
{
    // Fallout 4 voice playback for remote dialogue is not implemented yet.
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

TESForm* Actor::GetEquippedWeapon(uint32_t aSlotId) const noexcept
{
    if (!currentProcess || !currentProcess->middleProcess)
        return nullptr;

    for (const auto& item : currentProcess->middleProcess->equippedItems)
    {
        if (item.equipIndex == aSlotId && item.object && item.object->formType == FormType::Weapon)
            return item.object;
    }
    return nullptr;
}

TESNPC* Actor::GetLeveledPick() const noexcept
{
    // Fallout 4 keeps one template per template-use flag; remote picks are not applied yet.
    return nullptr;
}

Factions Actor::GetFactions() const noexcept
{
    Factions result;

    auto& modSystem = World::Get().GetModSystem();

    if (auto* pNpc = Cast<TESNPC>(baseForm))
    {
        for (const auto& entry : pNpc->actorData.factions)
        {
            Faction faction;
            modSystem.GetServerModId(entry.faction->formID, faction.Id);
            faction.Rank = entry.rank;
            result.NpcFactions.push_back(faction);
        }
    }

    const ExtraDataList* pExtraList = extraData;
    if (auto* pChanges = pExtraList ? static_cast<ExtraFactionChanges*>(pExtraList->GetByType(ExtraDataType::Faction)) : nullptr)
    {
        for (const auto& entry : pChanges->entries)
        {
            Faction faction;
            modSystem.GetServerModId(entry.faction->formID, faction.Id);
            faction.Rank = entry.rank;
            result.ExtraFactions.push_back(faction);
        }
    }

    return result;
}

void Actor::RemoveFromAllFactions() noexcept
{
    // A rank of -1 expels the actor from the faction.
    Factions factions = GetFactions();
    auto& modSystem = World::Get().GetModSystem();
    for (const auto* pList : {&factions.NpcFactions, &factions.ExtraFactions})
    {
        for (const auto& entry : *pList)
        {
            if (auto* pFaction = Cast<TESFaction>(TESForm::GetById(modSystem.GetGameId(entry.Id))))
                SetFactionRank(pFaction, -1);
        }
    }
}

void Actor::SetFactions(const Factions& acFactions) noexcept
{
    RemoveFromAllFactions();

    auto& modSystem = World::Get().GetModSystem();
    for (const auto* pList : {&acFactions.NpcFactions, &acFactions.ExtraFactions})
    {
        for (const auto& entry : *pList)
        {
            if (auto* pFaction = Cast<TESFaction>(TESForm::GetById(modSystem.GetGameId(entry.Id))))
                SetFactionRank(pFaction, entry.Rank);
        }
    }
}

void Actor::PickUpObject(TESObjectREFR* apObject, int32_t aCount, bool aPlaySounds, float) noexcept
{
    TP_THIS_FUNCTION(TPickUpObject, void, Actor, TESObjectREFR*, int32_t, bool);
    static VersionDbPtr<TPickUpObject> pickUpObject(2229956);
    TiltedPhoques::ThisCall(pickUpObject, this, apObject, aCount, aPlaySounds);
}

void Actor::UnEquipAll() noexcept
{
    TP_THIS_FUNCTION(TUnequipAll, void, Actor);
    static VersionDbPtr<TUnequipAll> unequipAll(2229944);
    TiltedPhoques::ThisCall(unequipAll, this);
}

Inventory Actor::GetActorInventory() const noexcept
{
    // Fallout 4 actors have no equipped spells or shouts to sync.
    return GetInventory();
}

Inventory Actor::GetEquipment() const noexcept
{
    Inventory inventory = GetInventory();
    inventory.RemoveByFilter([](const auto& entry) { return !entry.IsWorn(); });
    return inventory;
}

void Actor::SetActorInventory(const Inventory& acInventory) noexcept
{
    spdlog::info("Setting inventory for actor {:X}", formID);

    Inventory currentInventory = GetActorInventory();

    if (!GetExtension()->IsPlayer() && currentInventory.ContainsQuestItems())
        SetInventoryRetainingQuestItems(currentInventory, acInventory);
    else
        SetInventory(acInventory);
}

void Actor::DropOrPickUpObject(const Inventory::Entry& arEntry, NiPoint3* apLocation, NiPoint3* apRotation) noexcept
{
    auto& modSystem = World::Get().GetModSystem();

    auto* pObject = Cast<TESBoundObject>(TESForm::GetById(modSystem.GetGameId(arEntry.BaseId)));
    if (!pObject)
    {
        spdlog::warn("Object to drop not found, {:X}:{:X}.", arEntry.BaseId.ModId, arEntry.BaseId.BaseId);
        return;
    }

    // TODO: pick up
    if (arEntry.Count >= 0)
        return;

    RemoveItemData data{};
    *reinterpret_cast<uint32_t*>(data.stackData) = 0x80000000;
    data.object = pObject;
    data.count = -arEntry.Count;
    data.reason = ITEM_REMOVE_REASON::kDropping;
    data.dropLoc = apLocation;
    data.rotate = apRotation;
    RemoveItem(data);
}
