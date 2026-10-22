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

#include <Events/UpdateEvent.h>
#include <Events/InventoryChangeEvent.h>
#include <Events/EquipmentChangeEvent.h>

#include <World.h>
#include <Systems/OwnershipSystem.h>
#include <Games/Skyrim/Interface/UI.h>
#include <PlayerCharacter.h>
#include <Forms/TESObjectCELL.h>
#include <Actor.h>
#include <Structs/ObjectData.h>
#include <Forms/TESWorldSpace.h>
#include <Games/TES.h>
#include <Games/Overrides.h>
#include <EquipManager.h>
#include <Games/ActorExtension.h>
#include <Forms/TESNPC.h>
#include <DefaultObjectManager.h>

InventoryService::InventoryService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept
    : m_world(aWorld)
    , m_dispatcher(aDispatcher)
    , m_transport(aTransport)
{
    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&InventoryService::OnUpdate>(this);
    m_inventoryConnection = m_dispatcher.sink<InventoryChangeEvent>().connect<&InventoryService::OnInventoryChangeEvent>(this);
    m_equipmentConnection = m_dispatcher.sink<EquipmentChangeEvent>().connect<&InventoryService::OnEquipmentChangeEvent>(this);
    m_inventoryChangeConnection = m_dispatcher.sink<NotifyInventoryChanges>().connect<&InventoryService::OnNotifyInventoryChanges>(this);
    m_equipmentChangeConnection = m_dispatcher.sink<NotifyEquipmentChanges>().connect<&InventoryService::OnNotifyEquipmentChanges>(this);
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

    m_transport.Send(request);

    spdlog::info("Sending item request, item: {:X}, count: {}, target object: {:X}", acEvent.Item.BaseId.BaseId, acEvent.Item.Count, acEvent.FormId);
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

    m_transport.Send(request);

    spdlog::info("Sending equipment request, item: {:X}, count: {}, target object: {:X}", acEvent.ItemId, acEvent.Count, acEvent.ActorId);
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
        // Unequip all armor first, since the game won't auto unequip armor
        Inventory wornArmor{};
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

        bool isWeaponDrawn = pActor->actorState.IsWeaponDrawn();
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
