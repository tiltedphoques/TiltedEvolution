#include <Services/InventoryService.h>

#include <Messages/RequestObjectInventoryChanges.h>
#include <Messages/NotifyObjectInventoryChanges.h>
#include <Messages/RequestCharacterInventoryChanges.h>
#include <Messages/NotifyCharacterInventoryChanges.h>
#include <Messages/DrawWeaponRequest.h>
#include <Messages/NotifyDrawWeapon.h>

#include <Events/UpdateEvent.h>
#include <Events/InventoryChangeEvent.h>
#include <Events/EquipmentChangeEvent.h>

#include <World.h>
#include <Games/Misc/UI.h>
#include <PlayerCharacter.h>
#include <Forms/TESObjectCELL.h>
#include <Actor.h>
#include <Structs/ObjectData.h>
#include <Forms/TESWorldSpace.h>
#include <Games/TES.h>

InventoryService::InventoryService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept
    : m_world(aWorld)
    , m_dispatcher(aDispatcher)
    , m_transport(aTransport)
{
    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&InventoryService::OnUpdate>(this);
    m_inventoryConnection = m_dispatcher.sink<InventoryChangeEvent>().connect<&InventoryService::OnInventoryChangeEvent>(this);
    m_equipmentConnection = m_dispatcher.sink<EquipmentChangeEvent>().connect<&InventoryService::OnEquipmentChangeEvent>(this);
    m_objectInventoryChangeConnection = m_dispatcher.sink<NotifyObjectInventoryChanges>().connect<&InventoryService::OnObjectInventoryChanges>(this);
    m_characterInventoryChangeConnection = m_dispatcher.sink<NotifyCharacterInventoryChanges>().connect<&InventoryService::OnCharacterInventoryChanges>(this);
    m_drawWeaponConnection = m_dispatcher.sink<NotifyDrawWeapon>().connect<&InventoryService::OnNotifyDrawWeapon>(this);
}

void InventoryService::OnUpdate(const UpdateEvent& acUpdateEvent) noexcept
{
    RunObjectInventoryUpdates();
    RunCharacterInventoryUpdates();

    ApplyCachedObjectInventoryChanges();
    ApplyCachedCharacterInventoryChanges();

    RunWeaponStateUpdates();
}

void InventoryService::OnInventoryChangeEvent(const InventoryChangeEvent& acEvent) noexcept
{
    if (!m_transport.IsConnected())
        return;

    const auto* pForm = TESForm::GetById(acEvent.FormId);
    if (RTTI_CAST(pForm, TESForm, Actor))
    {
        m_charactersWithInventoryChanges[acEvent.FormId] = String{};
    }
    else
    {
        m_objectsWithInventoryChanges.insert(acEvent.FormId);
    }
}

void InventoryService::OnEquipmentChangeEvent(const EquipmentChangeEvent& acEvent) noexcept
{
    m_charactersWithInventoryChanges[acEvent.ActorId] = acEvent.InventoryBuffer;
}

void InventoryService::OnObjectInventoryChanges(const NotifyObjectInventoryChanges& acMessage) noexcept
{
    for (const auto& [id, inventory] : acMessage.Changes)
    {
        m_cachedObjectInventoryChanges[id] = inventory;
    }

    ApplyCachedObjectInventoryChanges();
}

void InventoryService::OnCharacterInventoryChanges(const NotifyCharacterInventoryChanges& acMessage) noexcept
{
    for (const auto& [id, inventory] : acMessage.Changes)
    {
        m_cachedCharacterInventoryChanges[id] = inventory;
    }

    ApplyCachedCharacterInventoryChanges();
}

void InventoryService::RunObjectInventoryUpdates() noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 250ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    if (!m_objectsWithInventoryChanges.empty())
    {
        RequestObjectInventoryChanges message;

        for (const auto objectId : m_objectsWithInventoryChanges)
        {
            const auto* pObject = RTTI_CAST(TESForm::GetById(objectId), TESForm, TESObjectREFR);

            if (!pObject)
                continue;

        #if TP_FALLOUT4
            if (!pObject->inventory)
                continue;
        #endif

            ObjectData objectData;

            GameId gameId(0, 0);
            if (!m_world.GetModSystem().GetServerModId(pObject->formID, gameId.ModId, gameId.BaseId))
                continue;

            if (const auto pWorldSpace = pObject->GetWorldSpace())
            {
                if (!m_world.GetModSystem().GetServerModId(pWorldSpace->formID, objectData.WorldSpaceId.ModId, objectData.WorldSpaceId.BaseId))
                    continue;

                const auto* pTES = TES::Get();
                const auto* pCell = ModManager::Get()->GetCellFromCoordinates(pTES->currentGridX, pTES->currentGridY, pWorldSpace, 0);
                if (!pCell)
                {
                    spdlog::warn("Cell not found for coordinates ({}, {}) in worldspace {:X}", pTES->currentGridX, pTES->currentGridY, pWorldSpace->formID);
                    continue;
                }

                if (!m_world.GetModSystem().GetServerModId(pCell->formID, objectData.CellId.ModId, objectData.CellId.BaseId))
                    continue;

                objectData.CurrentCoords = GridCellCoords(pTES->currentGridX, pTES->currentGridY);
            }
            else if (const auto pParentCell = pObject->GetParentCell())
            {
                if (!m_world.GetModSystem().GetServerModId(pParentCell->formID, objectData.CellId.ModId, objectData.CellId.BaseId))
                    continue;
            }

            objectData.CurrentInventory.Buffer = pObject->SerializeInventory();

            message.Changes[gameId] = objectData;
        }

        m_transport.Send(message);

        m_objectsWithInventoryChanges.clear();
    }
}

void InventoryService::RunCharacterInventoryUpdates() noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 250ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    if (!m_charactersWithInventoryChanges.empty())
    {
        RequestCharacterInventoryChanges message;

        for (const auto change : m_charactersWithInventoryChanges)
        {
            auto view = m_world.view<FormIdComponent>();

            auto formId = change.first;

            const auto iter = std::find_if(std::begin(view), std::end(view), [view, formId](auto entity) 
            {
                return view.get<FormIdComponent>(entity).Id == formId;
            });

            if (iter == std::end(view))
                continue;

            std::optional<uint32_t> serverIdRes = utils::GetServerId(*iter);
            if (!serverIdRes.has_value())
                continue;

            uint32_t serverId = serverIdRes.value();

            const auto* pForm = TESForm::GetById(formId);
            auto* pActor = RTTI_CAST(pForm, TESForm, Actor);
            if (!pActor)
                continue;

            Inventory inventory = pActor->GetInventory();

            if (change.second != String{})
            {
                spdlog::warn("Using cached inventory snapshot for {:X} ({:X})", formId, serverId);
                //inventory.Buffer = change.second;
            }
            else
            {
                spdlog::warn("Using new inventory for {:X} ({:X})", formId, serverId);
            }

            message.Changes[serverId] = inventory;
        }

        m_transport.Send(message);

        m_charactersWithInventoryChanges.clear();
    }
}

void InventoryService::ApplyCachedObjectInventoryChanges() noexcept
{
    if (UI::Get()->IsOpen(BSFixedString("ContainerMenu")))
        return;

    for (const auto& [id, inventory] : m_cachedObjectInventoryChanges)
    {
        const auto cObjectId = World::Get().GetModSystem().GetGameId(id);
        if (cObjectId == 0)
        {
            spdlog::error("Failed to retrieve object to sync inventory.");
            continue;
        }

        auto* pObject = RTTI_CAST(TESForm::GetById(cObjectId), TESForm, TESObjectREFR);
        if (!pObject)
        {
            spdlog::error("Failed to retrieve object to sync inventory.");
            continue;
        }

        pObject->RemoveAllItems();
        pObject->DeserializeInventory(inventory.Buffer);
    }

    m_cachedObjectInventoryChanges.clear();
}

void InventoryService::ApplyCachedCharacterInventoryChanges() noexcept
{
    if (!m_transport.IsConnected())
        return;

    if (UI::Get()->IsOpen(BSFixedString("ContainerMenu")))
        return;

    auto view = m_world.view<FormIdComponent>();
    for (const auto entity : view)
    {
        std::optional<uint32_t> serverIdRes = utils::GetServerId(entity);
        if (!serverIdRes.has_value())
            continue;

        uint32_t serverId = serverIdRes.value();

        const auto change = m_cachedCharacterInventoryChanges.find(serverId);

        if (change == m_cachedCharacterInventoryChanges.end())
            continue;

        const auto& formIdComponent = view.get<FormIdComponent>(entity);
        auto* const pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);
        if (!pActor)
            continue;

        auto* cpRemoteComponent = m_world.try_get<RemoteComponent>(entity);
        if (cpRemoteComponent)
            cpRemoteComponent->SpawnRequest.InventoryContent = change.value();

        pActor->SetInventory(change.value());
    }

    m_cachedCharacterInventoryChanges.clear();
}

void InventoryService::RunWeaponStateUpdates() noexcept
{
    if (!m_transport.IsConnected())
        return;

    static std::chrono::steady_clock::time_point lastSendTimePoint;
    // TODO: profile how often this could run
    constexpr auto cDelayBetweenUpdates = 100ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenUpdates)
        return;

    lastSendTimePoint = now;

    // TODO: find_if()
    auto view = m_world.view<FormIdComponent, LocalComponent>();

    for (auto entity : view)
    {
        const auto& formIdComponent = view.get<FormIdComponent>(entity);
        auto* const pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);
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

void InventoryService::OnNotifyDrawWeapon(const NotifyDrawWeapon& acMessage) noexcept
{
    auto view = m_world.view<FormIdComponent, RemoteComponent>();

    const auto it = std::find_if(std::begin(view), std::end(view), [id = acMessage.Id, view](entt::entity entity) {
        return view.get<RemoteComponent>(entity).Id == id;
    });

    if (it == std::end(view))
        return;

    auto& formIdComponent = view.get<FormIdComponent>(*it);
    Actor* pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);

    if (!pActor)
        return;

    if (pActor->actorState.IsWeaponDrawn() != acMessage.IsWeaponDrawn)
        pActor->SetWeaponDrawnEx(acMessage.IsWeaponDrawn);
}

