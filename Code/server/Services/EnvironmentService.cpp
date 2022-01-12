#include <stdafx.h>


#include <GameServer.h>
#include <World.h>

#include <Services/EnvironmentService.h>
#include <Events/UpdateEvent.h>
#include <Events/PlayerJoinEvent.h>
#include <Events/PlayerLeaveCellEvent.h>
#include <Messages/ServerTimeSettings.h>
#include <Messages/ActivateRequest.h>
#include <Messages/NotifyActivate.h>
#include <Messages/LockChangeRequest.h>
#include <Messages/NotifyLockChange.h>
#include <Messages/AssignObjectsRequest.h>
#include <Messages/AssignObjectsResponse.h>
#include <Messages/ScriptAnimationRequest.h>
#include <Messages/NotifyScriptAnimation.h>
#include <Components.h>

EnvironmentService::EnvironmentService(World &aWorld, entt::dispatcher &aDispatcher) : m_world(aWorld)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&EnvironmentService::OnUpdate>(this);
    m_joinConnection = aDispatcher.sink<PlayerJoinEvent>().connect<&EnvironmentService::OnPlayerJoin>(this);
    m_leaveCellConnection = aDispatcher.sink<PlayerLeaveCellEvent>().connect<&EnvironmentService::OnPlayerLeaveCellEvent>(this);
    m_assignObjectConnection = aDispatcher.sink<PacketEvent<AssignObjectsRequest>>().connect<&EnvironmentService::OnAssignObjectsRequest>(this);
    m_activateConnection = aDispatcher.sink<PacketEvent<ActivateRequest>>().connect<&EnvironmentService::OnActivate>(this);
    m_lockChangeConnection = aDispatcher.sink<PacketEvent<LockChangeRequest>>().connect<&EnvironmentService::OnLockChange>(this);
    m_scriptAnimationConnection = aDispatcher.sink<PacketEvent<ScriptAnimationRequest>>().connect<&EnvironmentService::OnScriptAnimationRequest>(this);
}

void EnvironmentService::OnPlayerJoin(const PlayerJoinEvent& acEvent) const noexcept
{
    ServerTimeSettings timeMsg;
    timeMsg.TimeScale = m_timeModel.TimeScale;
    timeMsg.Time = m_timeModel.Time;

    acEvent.pPlayer->Send(timeMsg);
}

void EnvironmentService::OnPlayerLeaveCellEvent(const PlayerLeaveCellEvent& acEvent) noexcept
{
    for (auto pPlayer : m_world.GetPlayerManager())
    {
        if (pPlayer->GetCellComponent().Cell == acEvent.OldCell)
            return;
    }

    auto objectView = m_world.view<ObjectComponent, CellIdComponent>();
    for (auto entity : objectView)
    {
        const auto& cellIdComponent = objectView.get<CellIdComponent>(entity);

        if (cellIdComponent.Cell != acEvent.OldCell)
            continue;

        m_world.destroy(entity);
    }
}

void EnvironmentService::OnAssignObjectsRequest(const PacketEvent<AssignObjectsRequest>& acMessage) noexcept
{
    auto view = m_world.view<FormIdComponent, ObjectComponent, InventoryComponent>();

    AssignObjectsResponse response;

    for (const auto& object : acMessage.Packet.Objects)
    {
        const auto iter = std::find_if(std::begin(view), std::end(view), [view, id = object.Id](auto entity)
        {
            const auto formIdComponent = view.get<FormIdComponent>(entity);
            return formIdComponent.Id == id;
        });

        if (iter != std::end(view))
        {
            ObjectData objectData;

            auto& formIdComponent = view.get<FormIdComponent>(*iter);
            objectData.Id = formIdComponent.Id;

            auto& objectComponent = view.get<ObjectComponent>(*iter);
            objectData.CurrentLockData = objectComponent.CurrentLockData;

            auto& inventoryComponent = view.get<InventoryComponent>(*iter);
            objectData.CurrentInventory.Buffer = inventoryComponent.Content.Buffer;

            response.Objects.push_back(objectData);
        }
        else
        {
            const auto cEntity = m_world.create();

            m_world.emplace<FormIdComponent>(cEntity, object.Id);

            auto& objectComponent = m_world.emplace<ObjectComponent>(cEntity, acMessage.pPlayer);
            objectComponent.CurrentLockData = object.CurrentLockData;

            m_world.emplace<CellIdComponent>(cEntity, object.CellId, object.WorldSpaceId, object.CurrentCoords);
            m_world.emplace<InventoryComponent>(cEntity);
        }
    }

    if (!response.Objects.empty())
        acMessage.pPlayer->Send(response);
}

void EnvironmentService::OnActivate(const PacketEvent<ActivateRequest>& acMessage) const noexcept
{
    NotifyActivate notifyActivate;
    notifyActivate.Id = acMessage.Packet.Id;
    notifyActivate.ActivatorId = acMessage.Packet.ActivatorId;

    for (auto pPlayer : m_world.GetPlayerManager())
    {
        if (pPlayer != acMessage.pPlayer && pPlayer->GetCellComponent().Cell == acMessage.Packet.CellId)
        {
            pPlayer->Send(notifyActivate);
        }
    }
}

void EnvironmentService::OnLockChange(const PacketEvent<LockChangeRequest>& acMessage) const noexcept
{
    NotifyLockChange notifyLockChange;
    notifyLockChange.Id = acMessage.Packet.Id;
    notifyLockChange.IsLocked = acMessage.Packet.IsLocked;
    notifyLockChange.LockLevel = acMessage.Packet.LockLevel;

    auto objectView = m_world.view<FormIdComponent, ObjectComponent>();

    const auto iter = std::find_if(std::begin(objectView), std::end(objectView), [objectView, id = acMessage.Packet.Id](auto entity)
    {
        const auto formIdComponent = objectView.get<FormIdComponent>(entity);
        return formIdComponent.Id == id;
    });

    if (iter != std::end(objectView))
    {
        auto& objectComponent = objectView.get<ObjectComponent>(*iter);
        objectComponent.CurrentLockData.IsLocked = acMessage.Packet.IsLocked;
        objectComponent.CurrentLockData.LockLevel = acMessage.Packet.LockLevel;
    }

    for(auto pPlayer : m_world.GetPlayerManager())
    {
        if (pPlayer != acMessage.pPlayer && pPlayer->GetCellComponent().Cell == acMessage.Packet.CellId)
        {
            pPlayer->Send(notifyLockChange);
        }
    }
}

bool EnvironmentService::SetTime(int aHours, int aMinutes, float aScale) noexcept
{
    m_timeModel.TimeScale = aScale;

    if (aHours >= 0 && aHours <= 24 && aMinutes >= 0 && aMinutes <= 60)
    {
        // encode time as skyrim time
        auto minutes = static_cast<float>(aMinutes) * 0.17f;
        minutes = floor(minutes * 100) / 1000;
        m_timeModel.Time = static_cast<float>(aHours) + minutes;

        ServerTimeSettings timeMsg;
        timeMsg.TimeScale = m_timeModel.TimeScale;
        timeMsg.Time = m_timeModel.Time;
        GameServer::Get()->SendToLoaded(timeMsg);
        return true;
    }

    return false;
}

EnvironmentService::TTime EnvironmentService::GetTime() const noexcept
{
    const auto hour = floor(m_timeModel.Time);
    const auto minutes = (m_timeModel.Time - hour) / 17.f;

    const auto flatMinutes = static_cast<int>(ceil((minutes * 100.f) * 10.f));
    return {static_cast<int>(hour), flatMinutes};
}

EnvironmentService::TTime EnvironmentService::GetRealTime() noexcept
{
    const auto t = std::time(nullptr);
    int h = (t / 3600) % 24;
    int m = (t / 60) % 60;
    return {h, m};
}

EnvironmentService::TDate EnvironmentService::GetDate() const noexcept
{
    return {m_timeModel.Day, m_timeModel.Month, m_timeModel.Year};
}

void EnvironmentService::OnUpdate(const UpdateEvent &) noexcept
{
    if (!m_lastTick)
        m_lastTick = GameServer::Get()->GetTick();

    auto now = GameServer::Get()->GetTick();

    // client got ahead, we wait
    if (now < m_lastTick)
        return;

    auto delta = now - m_lastTick;
    m_lastTick = now;
    m_timeModel.Update(delta);
}

void EnvironmentService::OnScriptAnimationRequest(const PacketEvent<ScriptAnimationRequest>& acMessage) noexcept
{
    auto packet = acMessage.Packet;

    NotifyScriptAnimation message{};
    message.FormID = packet.FormID;
    message.Animation = packet.Animation;
    message.EventName = packet.EventName;

    for(auto pPlayer : m_world.GetPlayerManager())
    {
        pPlayer->Send(message);
    }
}
