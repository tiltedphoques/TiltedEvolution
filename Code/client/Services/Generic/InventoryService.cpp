#include <Services/InventoryService.h>

#include <Messages/RequestObjectInventoryChanges.h>
#include <Messages/NotifyObjectInventoryChanges.h>
#include <Messages/RequestInventoryChanges.h>
#include <Messages/NotifyInventoryChanges.h>
#include <Messages/RequestEquipmentChanges.h>
#include <Messages/NotifyEquipmentChanges.h>
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
#include <Games/Overrides.h>

InventoryService::InventoryService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept
    : m_world(aWorld)
    , m_dispatcher(aDispatcher)
    , m_transport(aTransport)
{
    m_updateConnection = m_dispatcher.sink<UpdateEvent>().connect<&InventoryService::OnUpdate>(this);
    m_inventoryConnection = m_dispatcher.sink<InventoryChangeEvent>().connect<&InventoryService::OnInventoryChangeEvent>(this);
    m_equipmentConnection = m_dispatcher.sink<EquipmentChangeEvent>().connect<&InventoryService::OnEquipmentChangeEvent>(this);
    m_inventoryChangeConnection = m_dispatcher.sink<NotifyInventoryChanges>().connect<&InventoryService::OnNotifyInventoryChanges>(this);
    m_drawWeaponConnection = m_dispatcher.sink<NotifyDrawWeapon>().connect<&InventoryService::OnNotifyDrawWeapon>(this);
    m_equipmentChangeConnection = m_dispatcher.sink<NotifyEquipmentChanges>().connect<&InventoryService::OnNotifyEquipmentChanges>(this);
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

    const auto iter = std::find_if(std::begin(view), std::end(view), [view, formId = acEvent.FormId](auto entity) 
    {
        return view.get<FormIdComponent>(entity).Id == formId;
    });

    if (iter == std::end(view))
        return;

    std::optional<uint32_t> serverIdRes = Utils::GetServerId(*iter);
    if (!serverIdRes.has_value())
        return;

    RequestInventoryChanges request;
    request.ServerId = serverIdRes.value();
    request.Item = std::move(acEvent.Item);
    request.DropOrPickUp = acEvent.DropOrPickUp;

    m_transport.Send(request);

    spdlog::info("Sending item request, item: {:X}, count: {}, target object: {:X}", acEvent.Item.BaseId.BaseId, acEvent.Item.Count,
                 acEvent.FormId);
}

void InventoryService::OnEquipmentChangeEvent(const EquipmentChangeEvent& acEvent) noexcept
{
    if (!m_transport.IsConnected())
        return;

    auto view = m_world.view<FormIdComponent>();

    const auto iter = std::find_if(std::begin(view), std::end(view), [view, formId = acEvent.ActorId](auto entity) 
    {
        return view.get<FormIdComponent>(entity).Id == formId;
    });

    if (iter == std::end(view))
        return;

    std::optional<uint32_t> serverIdRes = Utils::GetServerId(*iter);
    if (!serverIdRes.has_value())
        return;

    Actor* pActor = RTTI_CAST(TESForm::GetById(acEvent.ActorId), TESForm, Actor);
    if (!pActor)
        return;

    RequestEquipmentChanges request;
    request.ServerId = serverIdRes.value();
    request.CurrentEquipment = pActor->GetEquipment();

    m_transport.Send(request);

    spdlog::info("Sending equipment request, actor: {:X}", acEvent.ActorId);
}

void InventoryService::OnNotifyInventoryChanges(const NotifyInventoryChanges& acMessage) noexcept
{
    if (acMessage.DropOrPickUp)
    {
        Actor* pActor = GetByServerId(Actor, acMessage.ServerId);
        if (!pActor)
            return;

        ScopedInventoryOverride _;

        ExtraDataList* pExtraData = pActor->GetExtraDataFromItem(acMessage.Item);
        ModSystem& modSystem = World::Get().GetModSystem();

        uint32_t objectId = modSystem.GetGameId(acMessage.Item.BaseId);
        TESBoundObject* pObject = RTTI_CAST(TESForm::GetById(objectId), TESForm, TESBoundObject);
        if (!pObject)
        {
            spdlog::warn("{}: Object to drop not found, {:X}:{:X}.", __FUNCTION__, acMessage.Item.BaseId.ModId,
                         acMessage.Item.BaseId.BaseId);
            return;
        }

        if (acMessage.Item.Count < 0)
            pActor->DropObject(pObject, pExtraData, acMessage.Item.Count, nullptr, nullptr);
    }
    else
    {
        TESObjectREFR* pObject = GetByServerId(TESObjectREFR, acMessage.ServerId);
        if (!pObject)
            return;

        ScopedInventoryOverride _;

        pObject->AddOrRemoveItem(acMessage.Item);
    }
}

void InventoryService::OnNotifyEquipmentChanges(const NotifyEquipmentChanges& acMessage) noexcept
{
    Actor* pActor = GetByServerId(Actor, acMessage.ServerId);
    if (!pActor)
        return;

    pActor->SetEquipment(acMessage.CurrentEquipment);
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

