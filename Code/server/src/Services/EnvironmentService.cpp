
#include <GameServer.h>

#include <Services/EnvironmentService.h>
#include <Events/UpdateEvent.h>
#include <Events/PlayerJoinEvent.h>
#include <Messages/ServerTimeSettings.h>
#include <Components.h>

EnvironmentService::EnvironmentService(World &aWorld, entt::dispatcher &aDispatcher) : m_world(aWorld)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&EnvironmentService::OnUpdate>(this);
    m_joinConnection = aDispatcher.sink<PlayerJoinEvent>().connect<&EnvironmentService::OnPlayerJoin>(this);
}

void EnvironmentService::OnPlayerJoin(const PlayerJoinEvent& acEvent) noexcept
{
    ServerTimeSettings timeMsg;
    timeMsg.TimeScale = m_timeModel.TimeScale;
    timeMsg.Time = m_timeModel.Time;

    const auto &playerComponent = m_world.get<PlayerComponent>(acEvent.Entity);
    GameServer::Get()->Send(playerComponent.ConnectionId, timeMsg);
}

bool EnvironmentService::SetTime(int aHours, int aMinutes, float aScale) noexcept
{
    m_timeModel.TimeScale = aScale;

    if (aHours >= 0 && aHours <= 24 && aMinutes >= 0 && aMinutes <= 60)
    {
        // encode time as skyrim time
        float Min = static_cast<float>(aMinutes) * 0.17f;
        Min = floor(Min * 100) / 1000;
        m_timeModel.Time = static_cast<float>(aHours) + Min;

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
    float hour = floor(m_timeModel.Time);
    float minutes = (m_timeModel.Time - hour) / 17.f;

    int flatMinutes = static_cast<int>(ceil((minutes * 100.f) * 10.f));
    return {static_cast<int>(hour), flatMinutes};
}

EnvironmentService::TTime EnvironmentService::GetRealTime() noexcept
{
    auto t = std::time(nullptr);
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
