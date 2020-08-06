
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
    auto *pServer = GameServer::Get();

    ServerTimeSettings timeMsg;
    timeMsg.TimeScale = m_timeModel.TimeScale;
    timeMsg.Time = m_timeModel.Time;

    const auto &playerComponent = m_world.get<PlayerComponent>(acEvent.Entity);
    pServer->Send(playerComponent.ConnectionId, timeMsg);
}

bool EnvironmentService::SetTime(int Hour, int Minutes, float Scale)
{
    m_timeModel.TimeScale = Scale;

    if (Hour >= 0 && Hour <= 24 && Minutes >= 0 && Minutes <= 60)
    {
        // encode time as skyrim time
        float Min = static_cast<float>(Minutes) * 0.17f;
        Min = floor(Min * 100) / 1000;
        m_timeModel.Time = static_cast<float>(Hour) + Min;

        auto *pServer = GameServer::Get();

        ServerTimeSettings timeMsg;
        timeMsg.TimeScale = m_timeModel.TimeScale;
        timeMsg.Time = m_timeModel.Time;
        pServer->SendToLoaded(timeMsg);
        return true;
    }

    return false;
}

std::pair<int, int> EnvironmentService::GetTime()
{
    float Hour = floor(m_timeModel.Time);
    float Minutes = (m_timeModel.Time - Hour) / 17.f;

    int iHour = static_cast<int>(Hour);
    int iMinutes = static_cast<int>(ceil((Minutes * 100.f) * 10.f));

    return {iHour, iMinutes};
}

std::tuple<int, int, int> EnvironmentService::GetDate()
{
    // return the Date in a **reasonable** format
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
