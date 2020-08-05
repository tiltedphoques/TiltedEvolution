
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
    timeMsg.ServerTick = pServer->GetTickRate();
    timeMsg.TimeScale = m_timeScale;
    timeMsg.Time = m_time;

    spdlog::warn("I'm sending the timeprojection : {}:{}:{}", m_time, m_timeScale, pServer->GetTickRate());
    const auto &Player = m_world.get<PlayerComponent>(acEvent.Entity);
    pServer->Send(Player.ConnectionId, timeMsg);
}

bool EnvironmentService::SetTime(int Hour, int Minutes, float Scale)
{
    m_timeScale = Scale;

    if (Hour >= 0 && Hour <= 24 && Minutes >= 0 && Minutes <= 60)
    {
        // encode time as skyrim time
        // *i know* this is technically not 100% accurate
        float Min = static_cast<float>(Minutes) * 0.17f;
        Min = floor(Min * 100) / 1000;
        m_time = static_cast<float>(Hour) + Min;

        auto *pServer = GameServer::Get();
        ServerTimeSettings timeMsg;
        timeMsg.ServerTick = pServer->GetTickRate();
        timeMsg.TimeScale = m_timeScale;
        timeMsg.Time = m_time;
        pServer->SendToLoaded(timeMsg);
        return true;
    }

    return false;
}

std::pair<int, int> EnvironmentService::GetTime()
{
    float Hour = floor(m_time);
    float Minutes = (m_time - Hour) / 17.f;

    int iHour = static_cast<int>(Hour);
    int iMinutes = static_cast<int>(ceil((Minutes * 100.f) * 10.f));

    return {iHour, iMinutes};
}

std::tuple<int, int, int> EnvironmentService::GetDate()
{
    // return the Date in a **reasonable** format
    return {m_day, m_month, m_year};
}

static const int cDayLengthArray[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static int GetNumerOfDaysByMonthIndex(int index)
{
    if (index <= 12)
    {
        return cDayLengthArray[index];
    }

    return 0;
}

void EnvironmentService::OnUpdate(const UpdateEvent &aEvent) noexcept
{
    // update server time projection
    m_time = (aEvent.Delta * (m_timeScale * 0.00027777778f)) + m_time;
    if (m_time > 24.f)
    {
        int maxDays = GetNumerOfDaysByMonthIndex(m_month);

        while (m_time > 24.f)
        {
            m_time = m_time + -24.f;
            m_day++;
        }

        if (m_day > maxDays)
        {
            m_month++;
            m_day = m_day - maxDays;

            if (m_month > 12)
            {
                m_month = m_month + -12;
                m_year++;
            }
        }
    }

    std::printf("Time %f\n", m_time);
}
