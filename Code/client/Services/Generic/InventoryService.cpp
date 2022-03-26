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
#include <Games/Skyrim/Interface/UI.h>
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
    ApplyCachedObjectInventoryChanges();

    RunWeaponStateUpdates();
}

void InventoryService::OnInventoryChangeEvent(const InventoryChangeEvent& acEvent) noexcept
{
    if (!m_transport.IsConnected())
        return;

    const TESForm* pForm = TESForm::GetById(acEvent.FormId);
    if (!RTTI_CAST(pForm, TESForm, Actor))
    {
        return;
    }

    auto view = m_world.view<FormIdComponent>();

    const auto iter = std::find_if(std::begin(view), std::end(view), [view, formId = acEvent.FormId](auto entity) 
    {
        return view.get<FormIdComponent>(entity).Id == formId;
    });

    if (iter == std::end(view))
        return;

    std::optional<uint32_t> serverIdRes = Utils::GetServerId(*iter);
    if (!serverIdRes.has_value())
        return;

    RequestCharacterInventoryChanges request;
    request.ActorId = serverIdRes.value();
    request.Item = std::move(acEvent.Item);

    m_transport.Send(request);

    spdlog::info("Sending item request, item: {:X}, count: {}, actor: {:X}", acEvent.Item.BaseId.BaseId, acEvent.Item.Count,
                 acEvent.FormId);
}

void InventoryService::OnEquipmentChangeEvent(const EquipmentChangeEvent& acEvent) noexcept
{
}

void InventoryService::OnObjectInventoryChanges(const NotifyObjectInventoryChanges& acMessage) noexcept
{
    for (const auto& [id, inventory] : acMessage.Changes)
    {
        m_cachedObjectInventoryChanges[id] = inventory;
    }

    ApplyCachedObjectInventoryChanges();
}

extern thread_local bool g_modifyingInventory;

void InventoryService::OnCharacterInventoryChanges(const NotifyCharacterInventoryChanges& acMessage) noexcept
{
    std::optional<Actor*> actorResult = Utils::GetActorByServerId(acMessage.ActorId);
    if (!actorResult.has_value())
        return;

    Actor* pActor = actorResult.value();

    g_modifyingInventory = true;
    pActor->AddOrRemoveItem(acMessage.Item);
    g_modifyingInventory = false;
}

void InventoryService::ApplyCachedObjectInventoryChanges() noexcept
{
    if (!m_transport.IsConnected())
        return;

    if (UI::Get()->GetMenuOpen(BSFixedString("ContainerMenu")))
        return;

    for (auto& idInventory : m_cachedObjectInventoryChanges)
    {
        const uint32_t cObjectId = World::Get().GetModSystem().GetGameId(idInventory.first);
        if (cObjectId == 0)
        {
            spdlog::error("Failed to retrieve object to sync inventory.");
            continue;
        }

        TESObjectREFR* pObject = RTTI_CAST(TESForm::GetById(cObjectId), TESForm, TESObjectREFR);
        if (!pObject)
        {
            spdlog::error("Failed to retrieve object to sync inventory.");
            continue;
        }

        Inventory inventory = idInventory.second;
        pObject->SetInventory(inventory);
    }

    m_cachedObjectInventoryChanges.clear();
}

void InventoryService::ApplyCachedCharacterInventoryChanges() noexcept
{
    if (!m_transport.IsConnected())
        return;

    if (UI::Get()->GetMenuOpen(BSFixedString("ContainerMenu")))
        return;

    auto view = m_world.view<FormIdComponent, RemoteComponent>();
    for (auto& idInventory : m_cachedCharacterInventoryChanges)
    {
        const auto it = std::find_if(std::begin(view), std::end(view), [id = idInventory.first, view](entt::entity entity) { 
            return view.get<RemoteComponent>(entity).Id == id;
        });

        if (it == std::end(view))
            continue;

        const auto& formIdComponent = view.get<FormIdComponent>(*it);
        Actor* const pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);
        if (!pActor)
            continue;

        Inventory inventory = idInventory.second;

        auto& remoteComponent = m_world.get<RemoteComponent>(*it);
        remoteComponent.SpawnRequest.InventoryContent = inventory;

        pActor->SetActorInventory(inventory);
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
        Actor* const pActor = RTTI_CAST(TESForm::GetById(formIdComponent.Id), TESForm, Actor);
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

