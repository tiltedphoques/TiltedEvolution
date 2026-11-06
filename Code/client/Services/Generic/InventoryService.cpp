#include <Services/InventoryService.h>

#include <Messages/RequestObjectInventoryChanges.h>
#include <Messages/NotifyObjectInventoryChanges.h>
#include <Messages/RequestInventoryChanges.h>
#include <Messages/NotifyInventoryChanges.h>
#include <Messages/RequestEquipmentChanges.h>
#include <Messages/NotifyEquipmentChanges.h>
#include <Messages/NotifyInventorySnapshot.h>
#include <Messages/DrawWeaponRequest.h>
#include <Messages/NotifyDrawWeapon.h>
#include <Messages/RequestDroppedItemPickup.h>
#include <Messages/NotifyDroppedItemPickup.h>
#include <Messages/NotifyDroppedItem.h>

#include <Events/UpdateEvent.h>
#include <Events/InventoryChangeEvent.h>
#include <Events/EquipmentChangeEvent.h>
#include <Events/DroppedItemPickupEvent.h>
#include <Events/DisconnectedEvent.h>

#include <World.h>
#include <Systems/OwnershipSystem.h>
#include <Interface/UI.h>
#include <PlayerCharacter.h>
#include <Forms/TESObjectCELL.h>
#include <Actor.h>
#include <Structs/ObjectData.h>
#include <Forms/TESWorldSpace.h>
#include <Forms/TESBoundObject.h>
#include <Games/TES.h>
#include <Games/Overrides.h>
#include <EquipManager.h>
#include <Games/ActorExtension.h>
#include <Forms/TESNPC.h>
#include <DefaultObjectManager.h>

#include <random>

InventoryService::InventoryService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept
    : m_world(aWorld)
    , m_dispatcher(aDispatcher)
    , m_transport(aTransport)
    // Ids from an earlier session must not match drops on another server
    , m_nextDropId(std::random_device{}())
{
    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&InventoryService::OnUpdate>(this);
    m_inventoryConnection = m_dispatcher.sink<InventoryChangeEvent>().connect<&InventoryService::OnInventoryChangeEvent>(this);
    m_equipmentConnection = m_dispatcher.sink<EquipmentChangeEvent>().connect<&InventoryService::OnEquipmentChangeEvent>(this);
    m_inventoryChangeConnection = m_dispatcher.sink<NotifyInventoryChanges>().connect<&InventoryService::OnNotifyInventoryChanges>(this);
    m_equipmentChangeConnection = m_dispatcher.sink<NotifyEquipmentChanges>().connect<&InventoryService::OnNotifyEquipmentChanges>(this);
    m_droppedItemPickupConnection = m_dispatcher.sink<DroppedItemPickupEvent>().connect<&InventoryService::OnDroppedItemPickupEvent>(this);
    m_notifyDroppedItemPickupConnection = m_dispatcher.sink<NotifyDroppedItemPickup>().connect<&InventoryService::OnNotifyDroppedItemPickup>(this);
    m_notifyDroppedItemConnection = m_dispatcher.sink<NotifyDroppedItem>().connect<&InventoryService::OnNotifyDroppedItem>(this);
    m_disconnectedConnection = m_dispatcher.sink<DisconnectedEvent>().connect<&InventoryService::OnDisconnected>(this);
    m_inventorySnapshotConnection = m_dispatcher.sink<NotifyInventorySnapshot>().connect<&InventoryService::OnNotifyInventorySnapshot>(this);
}

void InventoryService::OnUpdate(const UpdateEvent& acUpdateEvent) noexcept
{
    RunWeaponStateUpdates();
}

void InventoryService::OnInventoryChangeEvent(const InventoryChangeEvent& acEvent) noexcept
{
    if (!m_transport.IsConnected())
        return;

    auto view = m_world.view<FormIdComponent>();

    const auto iter = std::find_if(std::begin(view), std::end(view), [view, formId = acEvent.FormId](auto entity) { return view.get<FormIdComponent>(entity).Id == formId; });

    if (iter == std::end(view))
        return;

    uint32_t serverId = 0;
    uint32_t ownershipEpoch = 0;
    if (Cast<Actor>(TESForm::GetById(acEvent.FormId)))
    {
        // The player's own changes to someone else's actor (looting, pickpocketing) are relayed to its owner
        const auto ownership = OwnershipSystem::GetOutgoingOwnership(m_world, *iter, acEvent.IsPlayerTransfer);
        if (!ownership)
        {
            spdlog::debug("Discarded an inventory change for actor {:X} that isn't owned locally (item {:X}, count {})", acEvent.FormId, acEvent.Item.BaseId.BaseId, acEvent.Item.Count);
            return;
        }

        serverId = ownership->ServerId;
        ownershipEpoch = ownership->OwnershipEpoch;
    }
    else
    {
        const std::optional<uint32_t> serverIdRes = Utils::GetServerId(*iter);
        if (!serverIdRes)
        {
            spdlog::warn(
                "Discarded inventory change for form {:X} because it has no server entity (item {:X}, count {})", acEvent.FormId, acEvent.Item.BaseId.BaseId, acEvent.Item.Count);
            return;
        }
        serverId = *serverIdRes;
    }

    RequestInventoryChanges request;
    request.ServerId = serverId;
    request.OwnershipEpoch = ownershipEpoch;
    request.Item = acEvent.Item;
    request.Drop = acEvent.Drop;
    request.UpdateClients = acEvent.UpdateClients;

    if (acEvent.Drop && acEvent.DroppedRefId)
    {
        auto* pDropped = Cast<TESObjectREFR>(TESForm::GetById(acEvent.DroppedRefId));
        if (pDropped && pDropped->baseForm)
        {
            // The server sends it back on reconnect, a saved copy would be a duplicate
            pDropped->SetSkipSaveFlag(true);
            request.DropId = ++m_nextDropId;
            request.DropPosition = glm::vec3(pDropped->position.x, pDropped->position.y, pDropped->position.z);
            m_droppedItems[acEvent.DroppedRefId] = {serverId, request.DropId, pDropped->baseForm->formID};
        }
    }

    m_transport.Send(request);

    spdlog::info("Sending item request, item: {:X}, count: {}, target object: {:X}, drop id: {}", acEvent.Item.BaseId.BaseId, acEvent.Item.Count, acEvent.FormId, request.DropId);
}

void InventoryService::OnEquipmentChangeEvent(const EquipmentChangeEvent& acEvent) noexcept
{
    if (!m_transport.IsConnected())
        return;

    auto view = m_world.view<FormIdComponent>();

    const auto iter = std::find_if(std::begin(view), std::end(view), [view, formId = acEvent.ActorId](auto entity) { return view.get<FormIdComponent>(entity).Id == formId; });

    if (iter == std::end(view))
        return;

    const auto ownership = OwnershipSystem::GetOutgoingOwnership(m_world, *iter, false);
    if (!ownership)
    {
        spdlog::debug("Discarded an equipment change for actor {:X} that isn't owned locally (item {:X})", acEvent.ActorId, acEvent.ItemId);
        return;
    }

    Actor* pActor = Cast<Actor>(TESForm::GetById(acEvent.ActorId));
    if (!pActor)
        return;

    auto& modSystem = World::Get().GetModSystem();

    RequestEquipmentChanges request;
    request.ServerId = ownership->ServerId;
    request.OwnershipEpoch = ownership->OwnershipEpoch;

    if (!modSystem.GetServerModId(acEvent.EquipSlotId, request.EquipSlotId))
        return;
    if (!modSystem.GetServerModId(acEvent.ItemId, request.ItemId))
        return;

    request.Count = acEvent.Count;
    request.Unequip = acEvent.Unequip;
    request.IsSpell = acEvent.IsSpell;
    request.IsShout = acEvent.IsShout;
    request.IsAmmo = acEvent.IsAmmo;
    request.CurrentInventory = pActor->GetEquipment();
    for (const uint32_t modId : acEvent.Mods)
        modSystem.GetServerModId(modId, request.Mods.emplace_back());

    m_transport.Send(request);

    spdlog::info("Sending equipment request, item: {:X}, count: {}, target object: {:X}, mods {}", acEvent.ItemId, acEvent.Count, acEvent.ActorId, acEvent.Mods.size());
}

void InventoryService::OnNotifyInventoryChanges(const NotifyInventoryChanges& acMessage) noexcept
{
    if (acMessage.OwnershipEpoch != 0)
    {
        Actor* pActor = nullptr;

        const auto remoteEntity = OwnershipSystem::AcceptRemoteUpdate(m_world, acMessage.ServerId, acMessage.OwnershipEpoch);
        if (remoteEntity)
        {
            // The spawn request's inventory replaces the actor's once its 3D loads, so the change has to go there too
            if (auto* pWaitingFor3D = m_world.try_get<WaitingFor3D>(*remoteEntity))
            {
                pWaitingFor3D->SpawnRequest.InventoryContent.AddOrRemoveEntry(acMessage.Item);
                if (!acMessage.Drop)
                    return;
            }

            if (const auto* pFormIdComponent = m_world.try_get<FormIdComponent>(*remoteEntity))
                pActor = Cast<Actor>(TESForm::GetById(pFormIdComponent->Id));
        }
        else if (const auto localEntity = OwnershipSystem::AcceptOwnerUpdate(m_world, acMessage.ServerId, acMessage.OwnershipEpoch))
        {
            // Remote interactions with our own actor, e.g. a player looting it, are relayed back to the owner.
            if (const auto* pFormIdComponent = m_world.try_get<FormIdComponent>(*localEntity))
                pActor = Cast<Actor>(TESForm::GetById(pFormIdComponent->Id));
        }

        if (!pActor)
        {
            spdlog::debug("Discarded an inventory update for actor {:X} because epoch {} is no longer current", acMessage.ServerId, acMessage.OwnershipEpoch);
            return;
        }

        ScopedInventoryOverride _;

        if (acMessage.Drop)
            pActor->DropOrPickUpObject(acMessage.Item, nullptr, nullptr);
        else
            pActor->AddOrRemoveItem(acMessage.Item);

        return;
    }

    TESObjectREFR* pObject = Utils::GetByServerId<TESObjectREFR>(acMessage.ServerId);
    if (!pObject)
        return;

    ScopedInventoryOverride _;
    pObject->AddOrRemoveItem(acMessage.Item);
}

void InventoryService::OnNotifyEquipmentChanges(const NotifyEquipmentChanges& acMessage) noexcept
{
    const auto entity = OwnershipSystem::AcceptRemoteUpdate(m_world, acMessage.ServerId, acMessage.OwnershipEpoch);
    if (!entity)
    {
        spdlog::debug("Discarded an equipment update for actor {:X} because epoch {} is no longer current", acMessage.ServerId, acMessage.OwnershipEpoch);
        return;
    }

    // Equipping before the 3D loads would be undone by the spawn request's inventory
    if (auto* pWaitingFor3D = m_world.try_get<WaitingFor3D>(*entity))
    {
        pWaitingFor3D->PendingEquipmentChanges.push_back(acMessage);
        return;
    }

    const auto* pFormIdComponent = m_world.try_get<FormIdComponent>(*entity);
    if (!pFormIdComponent)
        return;

    ApplyEquipmentChange(Cast<Actor>(TESForm::GetById(pFormIdComponent->Id)), acMessage);
}

void InventoryService::ApplyEquipmentChange(Actor* apActor, const NotifyEquipmentChanges& acMessage) noexcept
{
    if (!apActor)
        return;

    auto& modSystem = World::Get().GetModSystem();

    uint32_t itemId = modSystem.GetGameId(acMessage.ItemId);
    TESForm* pItem = TESForm::GetById(itemId);

    if (!pItem)
    {
        spdlog::error("Could not find inventory item {:X}:{:X}", acMessage.ItemId.ModId, acMessage.ItemId.BaseId);
        return;
    }

    uint32_t equipSlotId = modSystem.GetGameId(acMessage.EquipSlotId);
    TESForm* pEquipSlot = TESForm::GetById(equipSlotId);

    uint32_t slotId = 0;
    if (pEquipSlot == DefaultObjectManager::Get().rightEquipSlot)
        slotId = 1;

    auto* pEquipManager = EquipManager::Get();

    if (acMessage.IsSpell)
    {
        if (acMessage.Unequip)
            pEquipManager->UnEquipSpell(apActor, pItem, slotId);
        else
            pEquipManager->EquipSpell(apActor, pItem, slotId);

        return;
    }
    else if (acMessage.IsShout)
    {
        if (acMessage.Unequip)
            pEquipManager->UnEquipShout(apActor, pItem);
        else
            pEquipManager->EquipShout(apActor, pItem);

        return;
    }

    // TODO: ExtraData necessary? probably
    if (acMessage.Unequip)
    {
        pEquipManager->UnEquip(apActor, pItem, nullptr, acMessage.Count, pEquipSlot, false, true, false, false, nullptr);
    }
    else
    {
        // Unequip all armor first, since the game won't auto unequip armor.
        // Fallout 4 replaces items in the same body slots by itself, and putting the
        // old armor back would cover the new piece.
        Inventory wornArmor{};
#if !defined(TP_FALLOUT4)
        if (pItem->formType == FormType::Armor)
        {
            wornArmor = apActor->GetWornArmor();
            for (const auto& armor : wornArmor.Entries)
            {
                uint32_t armorId = modSystem.GetGameId(armor.BaseId);
                TESForm* pArmor = TESForm::GetById(armorId);
                if (pArmor)
                    pEquipManager->UnEquip(apActor, pArmor, nullptr, 1, pEquipSlot, false, true, false, false, nullptr);
            }
        }
#endif

#if defined(TP_FALLOUT4)
        if (auto* pObject = Cast<TESBoundObject>(pItem); pObject && !acMessage.Mods.empty())
        {
            if (!apActor->GetItemCountInInventory(pObject))
            {
                ExtraDataList* pExtra = nullptr;
                apActor->AddObjectToContainer(pObject, &pExtra, 1, nullptr, ITEM_REMOVE_REASON::kRemove);
            }
            apActor->AttachItemMods(pObject, acMessage.Mods);
        }
#endif

        pEquipManager->Equip(apActor, pItem, nullptr, acMessage.Count, pEquipSlot, false, true, false, false);

        for (const auto& armor : wornArmor.Entries)
        {
            uint32_t armorId = modSystem.GetGameId(armor.BaseId);
            TESForm* pArmor = TESForm::GetById(armorId);
            if (pArmor)
                pEquipManager->Equip(apActor, pArmor, nullptr, 1, pEquipSlot, false, true, false, false);
        }
    }
}

void InventoryService::OnNotifyInventorySnapshot(const NotifyInventorySnapshot& acMessage) noexcept
{
    const auto entity = OwnershipSystem::AcceptRemoteUpdate(m_world, acMessage.ServerId, acMessage.OwnershipEpoch);
    if (!entity)
    {
        spdlog::debug("Discarded an inventory snapshot for actor {:X} because epoch {} is no longer current", acMessage.ServerId, acMessage.OwnershipEpoch);
        return;
    }

    ApplyRemoteInventory(m_world, *entity, acMessage.CurrentInventory);
}

void InventoryService::ApplyRemoteInventory(World& aWorld, const entt::entity aEntity, const Inventory& acInventory) noexcept
{
    // The spawn request's inventory is applied once the 3D loads, anything applied before that would be overwritten
    if (auto* pWaitingFor3D = aWorld.try_get<WaitingFor3D>(aEntity))
    {
        pWaitingFor3D->SpawnRequest.InventoryContent = acInventory;
        pWaitingFor3D->PendingEquipmentChanges.clear();
        return;
    }

    const auto* pFormIdComponent = aWorld.try_get<FormIdComponent>(aEntity);
    Actor* pActor = pFormIdComponent ? Cast<Actor>(TESForm::GetById(pFormIdComponent->Id)) : nullptr;
    if (!pActor || !pActor->GetNiNode())
        return;

    const Inventory currentInventory = pActor->GetActorInventory();
    if (currentInventory.Entries != acInventory.Entries || currentInventory.CurrentMagicEquipment != acInventory.CurrentMagicEquipment)
        pActor->SetActorInventory(acInventory);
}

void InventoryService::RunWeaponStateUpdates() noexcept
{
    if (!m_transport.IsConnected())
        return;

    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenUpdates = 500ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenUpdates)
        return;

    lastSendTimePoint = now;

    auto view = m_world.view<FormIdComponent, LocalComponent>();

    for (auto entity : view)
    {
        const auto& formIdComponent = view.get<FormIdComponent>(entity);
        Actor* const pActor = Cast<Actor>(TESForm::GetById(formIdComponent.Id));
        auto& localComponent = view.get<LocalComponent>(entity);

        bool isWeaponDrawn = pActor->GetActorState()->IsWeaponDrawn();
        if (isWeaponDrawn != localComponent.IsWeaponDrawn)
        {
            localComponent.IsWeaponDrawn = isWeaponDrawn;

            DrawWeaponRequest request;
            request.Id = localComponent.Id;
            request.IsWeaponDrawn = isWeaponDrawn;

            m_transport.Send(request);
        }
    }
}

void InventoryService::OnDroppedItemPickupEvent(const DroppedItemPickupEvent& acEvent) noexcept
{
    const auto it = m_droppedItems.find(acEvent.RefId);
    if (it == m_droppedItems.end())
    {
        spdlog::debug("Picked up temporary reference {:X} that is not a synced drop", acEvent.RefId);
        return;
    }

    const DroppedItem item = it->second;
    m_droppedItems.erase(it);

    if (item.BaseId != acEvent.BaseId || !m_transport.IsConnected())
        return;

    // Kept until the server confirms, it gets taken back if someone else got there first
    Inventory::Entry entry{};
    World::Get().GetModSystem().GetServerModId(acEvent.BaseId, entry.BaseId);
    entry.Count = acEvent.Count;
    m_pendingPickups[MakeDropKey(item.DropperId, item.DropId)] = entry;

    RequestDroppedItemPickup request;
    request.DropperId = item.DropperId;
    request.DropId = item.DropId;
    m_transport.Send(request);

    spdlog::info("Picked up dropped item {:X}:{:X}", item.DropperId, item.DropId);
}

void InventoryService::OnNotifyDroppedItemPickup(const NotifyDroppedItemPickup& acMessage) noexcept
{
    if (acMessage.Result != NotifyDroppedItemPickup::kTaken)
    {
        const auto pending = m_pendingPickups.find(MakeDropKey(acMessage.DropperId, acMessage.DropId));
        if (pending == m_pendingPickups.end())
            return;

        if (acMessage.Result == NotifyDroppedItemPickup::kDenied)
        {
            Inventory::Entry entry = pending->second;
            entry.Count = -entry.Count;
            // Not overridden, so the removal is synced like any other
            PlayerCharacter::Get()->AddOrRemoveItem(entry);
            spdlog::info("Dropped item {:X}:{:X} was already picked up by someone else", acMessage.DropperId, acMessage.DropId);
        }
        m_pendingPickups.erase(pending);
        return;
    }

    const auto it = FindDroppedItem(acMessage.DropperId, acMessage.DropId);
    if (it == m_droppedItems.end())
        return;

    // Temporary form ids get reused once the reference is gone, so check it is still the same item
    if (auto* pDropped = GetDroppedReference(*it))
        pDropped->Delete();

    m_droppedItems.erase(it);
}

void InventoryService::OnNotifyDroppedItem(const NotifyDroppedItem& acMessage) noexcept
{
#if defined(TP_FALLOUT4)
    // Sent again on every cell entry, keep the copy we already have
    if (const auto it = FindDroppedItem(acMessage.DropperId, acMessage.DropId); it != m_droppedItems.end())
    {
        if (GetDroppedReference(*it))
        {
            spdlog::debug("Dropped item {:X}:{:X} is already placed", acMessage.DropperId, acMessage.DropId);
            return;
        }
        m_droppedItems.erase(it);
    }

    auto& modSystem = World::Get().GetModSystem();
    auto* pBase = Cast<TESBoundObject>(TESForm::GetById(modSystem.GetGameId(acMessage.Item.BaseId)));
    auto* pCell = acMessage.CellId ? Cast<TESObjectCELL>(TESForm::GetById(modSystem.GetGameId(acMessage.CellId))) : nullptr;
    auto* pWorld = acMessage.WorldSpaceId ? Cast<TESWorldSpace>(TESForm::GetById(modSystem.GetGameId(acMessage.WorldSpaceId))) : nullptr;
    if (!pBase || (!pCell && !pWorld))
    {
        spdlog::warn("Cannot place dropped item {:X}:{:X}, base {}, cell {}, worldspace {}", acMessage.DropperId, acMessage.DropId, fmt::ptr(pBase), fmt::ptr(pCell), fmt::ptr(pWorld));
        return;
    }

    NiPoint3 position{};
    position.x = acMessage.Position.x;
    position.y = acMessage.Position.y;
    position.z = acMessage.Position.z;
    NiPoint3 rotation{};

    auto* pDropped = TESObjectREFR::GetByHandle(ModManager::Get()->Spawn(position, rotation, pCell, pWorld, pBase));
    if (!pDropped)
        return;

    // The item belongs to whoever dropped it, not to this client's save
    pDropped->SetSkipSaveFlag(true);
    if (acMessage.Item.Count > 1 && pDropped->extraData)
        pDropped->extraData->SetCount(static_cast<int16_t>(std::min(acMessage.Item.Count, 0x7FFF)));

    m_droppedItems[pDropped->formID] = {acMessage.DropperId, acMessage.DropId, pBase->formID};
    spdlog::info("Placed dropped item {:X}:{:X} ({:X} x{}) as {:X}", acMessage.DropperId, acMessage.DropId, pBase->formID, acMessage.Item.Count, pDropped->formID);
#endif
}

TiltedPhoques::Map<uint32_t, InventoryService::DroppedItem>::iterator InventoryService::FindDroppedItem(uint32_t aDropperId, uint32_t aDropId) noexcept
{
    return std::find_if(m_droppedItems.begin(), m_droppedItems.end(), [aDropperId, aDropId](const auto& acPair)
                        { return acPair.second.DropperId == aDropperId && acPair.second.DropId == aDropId; });
}

TESObjectREFR* InventoryService::GetDroppedReference(const std::pair<const uint32_t, DroppedItem>& acEntry) noexcept
{
    auto* pDropped = Cast<TESObjectREFR>(TESForm::GetById(acEntry.first));
    if (!pDropped || pDropped->IsDeleted() || !pDropped->baseForm || pDropped->baseForm->formID != acEntry.second.BaseId)
        return nullptr;
    return pDropped;
}

// Dropped items stay known, so that the ones the server sends again on reconnect are not placed twice
void InventoryService::OnDisconnected(const DisconnectedEvent&) noexcept
{
    m_pendingPickups.clear();
}
