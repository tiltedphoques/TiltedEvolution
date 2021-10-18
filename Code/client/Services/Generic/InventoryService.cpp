#include <Services/InventoryService.h>

#include <Messages/RequestObjectInventoryChanges.h>
#include <Messages/NotifyObjectInventoryChanges.h>
#include <Messages/RequestCharacterInventoryChanges.h>
#include <Messages/NotifyCharacterInventoryChanges.h>

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
}

void InventoryService::OnUpdate(const UpdateEvent& acUpdateEvent) noexcept
{
    RunObjectInventoryUpdates();
    RunCharacterInventoryUpdates();

    ApplyCachedObjectInventoryChanges();
    ApplyCachedCharacterInventoryChanges();
}

void InventoryService::OnInventoryChangeEvent(const InventoryChangeEvent& acEvent) noexcept
{
    if (!m_transport.IsConnected())
        return;

    const auto* pForm = TESForm::GetById(acEvent.FormId);
    if (RTTI_CAST(pForm, TESForm, Actor))
    {
        m_charactersWithInventoryChanges.insert(acEvent.FormId);
    }
    else
    {
        m_objectsWithInventoryChanges.insert(acEvent.FormId);
    }
}

void InventoryService::OnEquipmentChangeEvent(const EquipmentChangeEvent& acEvent) noexcept
{
    m_charactersWithInventoryChanges.insert(acEvent.ActorId);
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

        for (const auto formId : m_charactersWithInventoryChanges)
        {
            auto view = m_world.view<FormIdComponent>();

            const auto iter = std::find_if(std::begin(view), std::end(view), [view, formId](auto entity) 
            {
                return view.get<FormIdComponent>(entity).Id == formId;
            });

            if (iter == std::end(view))
                continue;

            uint32_t componentId;
            const auto& cpLocalComponent = m_world.try_get<LocalComponent>(*iter);
            const auto& cpRemoteComponent = m_world.try_get<RemoteComponent>(*iter);

            if (cpLocalComponent)
                componentId = cpLocalComponent->Id;
            else if (cpRemoteComponent)
                componentId = cpRemoteComponent->Id;
            else
                continue;

            const auto* pForm = TESForm::GetById(formId);
            const auto* pActor = RTTI_CAST(pForm, TESForm, Actor);
            if (!pActor)
                continue;

            message.Changes[componentId] = pActor->GetInventory();
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
    if (UI::Get()->IsOpen(BSFixedString("ContainerMenu")))
        return;

    auto view = m_world.view<FormIdComponent>();
    for (const auto entity : view)
    {
        uint32_t componentId;
        const auto cpLocalComponent = m_world.try_get<LocalComponent>(entity);
        const auto cpRemoteComponent = m_world.try_get<RemoteComponent>(entity);

        if (cpLocalComponent)
            componentId = cpLocalComponent->Id;
        else if (cpRemoteComponent)
            componentId = cpRemoteComponent->Id;
        else
            continue;

        const auto change = m_cachedCharacterInventoryChanges.find(componentId);

        if (change == m_cachedCharacterInventoryChanges.end())
            continue;

        const auto& formIdComponent = view.get<FormIdComponent>(entity);
        auto* const pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);
        if (!pActor)
            continue;

        if (cpRemoteComponent)
            cpRemoteComponent->SpawnRequest.InventoryContent = change.value();

        pActor->SetInventory(change.value());
    }

    m_cachedCharacterInventoryChanges.clear();
}
