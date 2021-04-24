#include <stdafx.h>


#include <GameServer.h>

#include <Services/EnvironmentService.h>
#include <Events/UpdateEvent.h>
#include <Events/PlayerJoinEvent.h>
#include <Messages/ServerTimeSettings.h>
#include <Messages/ActivateRequest.h>
#include <Messages/NotifyActivate.h>
#include <Messages/LockChangeRequest.h>
#include <Messages/NotifyLockChange.h>
#include <Messages/AssignObjectsRequest.h>
#include <Messages/AssignObjectsResponse.h>
#include <Components.h>

EnvironmentService::EnvironmentService(World &aWorld, entt::dispatcher &aDispatcher) : m_world(aWorld)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&EnvironmentService::OnUpdate>(this);
    m_joinConnection = aDispatcher.sink<PlayerJoinEvent>().connect<&EnvironmentService::OnPlayerJoin>(this);
    m_assignObjectConnection = aDispatcher.sink<PacketEvent<AssignObjectsRequest>>().connect<&EnvironmentService::OnAssignObjectsRequest>(this);
    m_activateConnection = aDispatcher.sink<PacketEvent<ActivateRequest>>().connect<&EnvironmentService::OnActivate>(this);
    m_lockChangeConnection = aDispatcher.sink<PacketEvent<LockChangeRequest>>().connect<&EnvironmentService::OnLockChange>(this);
}

void EnvironmentService::OnPlayerJoin(const PlayerJoinEvent& acEvent) const noexcept
{
    ServerTimeSettings timeMsg;
    timeMsg.TimeScale = m_timeModel.TimeScale;
    timeMsg.Time = m_timeModel.Time;

    const auto &playerComponent = m_world.get<PlayerComponent>(acEvent.Entity);
    GameServer::Get()->Send(playerComponent.ConnectionId, timeMsg);
}

void EnvironmentService::OnAssignObjectsRequest(const PacketEvent<AssignObjectsRequest>& acMessage) noexcept
{
    auto view = m_world.view<FormIdComponent, LockComponent>();
    AssignObjectsResponse response;

    for (const auto& object : acMessage.Packet.Objects)
    {
        auto itor = m_objectsWithLocks.find(object.Id);
        if (itor != m_objectsWithLocks.end())
        {
            ObjectData objectData;

            auto& formIdComponent = view.get<FormIdComponent>(itor->second);
            objectData.Id = formIdComponent.Id;

            auto& lockComponent = view.get<LockComponent>(itor->second);
            objectData.CurrentLockData = lockComponent.CurrentLockData;

            response.Objects.push_back(objectData);
            continue;
        }

        const auto cEntity = m_world.create();
        m_world.emplace<FormIdComponent>(cEntity, object.Id);
        m_world.emplace<CellIdComponent>(cEntity, object.CellId);
        m_world.emplace<LockComponent>(cEntity, object.CurrentLockData);
        m_objectsWithLocks.insert({object.Id, cEntity});
    }

    if (!response.Objects.empty())
        GameServer::Get()->Send(acMessage.ConnectionId, response);
}

void EnvironmentService::OnActivate(const PacketEvent<ActivateRequest>& acMessage) const noexcept
{
    NotifyActivate notifyActivate;
    notifyActivate.Id = acMessage.Packet.Id;
    notifyActivate.ActivatorId = acMessage.Packet.ActivatorId;

    auto view = m_world.view<PlayerComponent, CellIdComponent>();
    for (auto entity : view)
    {
        auto& player = view.get<PlayerComponent>(entity);
        auto& cell = view.get<CellIdComponent>(entity);

        if (player.ConnectionId != acMessage.ConnectionId && cell.Cell == acMessage.Packet.CellId)
        {
            GameServer::Get()->Send(player.ConnectionId, notifyActivate);
        }
    }
}

void EnvironmentService::OnLockChange(const PacketEvent<LockChangeRequest>& acMessage) const noexcept
{
    NotifyLockChange notifyLockChange;
    notifyLockChange.Id = acMessage.Packet.Id;
    notifyLockChange.IsLocked = acMessage.Packet.IsLocked;
    notifyLockChange.LockLevel = acMessage.Packet.LockLevel;

    auto itor = m_objectsWithLocks.find(acMessage.Packet.Id);
    if (itor != m_objectsWithLocks.end())
    {
        auto objectView = m_world.view<LockComponent>();
        auto& lockComponent = objectView.get<LockComponent>(itor->second);
        lockComponent.CurrentLockData.IsLocked = acMessage.Packet.IsLocked;
        lockComponent.CurrentLockData.LockLevel = acMessage.Packet.LockLevel;
    }

    auto view = m_world.view<PlayerComponent, CellIdComponent>();
    for (auto entity : view)
    {
        auto& player = view.get<PlayerComponent>(entity);
        auto& cell = view.get<CellIdComponent>(entity);

        if (player.ConnectionId != acMessage.ConnectionId && cell.Cell == acMessage.Packet.CellId)
        {
            GameServer::Get()->Send(player.ConnectionId, notifyLockChange);
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
