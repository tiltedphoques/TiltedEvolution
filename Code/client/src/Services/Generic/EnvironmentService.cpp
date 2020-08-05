
#include <Events/UpdateEvent.h>
#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Services/EnvironmentService.h>
#include <Messages/ServerTimeSettings.h>

#include <Games/Skyrim/TimeManager.h>

EnvironmentService::EnvironmentService(entt::dispatcher &aDispatcher)
{
    m_timeUpdateConnection = aDispatcher.sink<ServerTimeSettings>().connect<&EnvironmentService::OnTimeUpdate>(this);
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&EnvironmentService::HandleUpdate>(this);
}

void EnvironmentService::OnTimeUpdate(const ServerTimeSettings &acMessage)
{
    // TODO: think about caching previous time and reapplying it on disconnect?
    m_timeScale = acMessage.TimeScale;
    m_time = acMessage.Time;
    m_enableMPtime = true;
}

void EnvironmentService::OnDisconnected(const DisconnectedEvent &acDisconnectedEvent) noexcept
{
    // clear time override
    m_enableMPtime = false;
}

static const int cDayLengthArray[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static int GetNumerOfDaysByMonthIndex(int index)
{
    if (index <= 12)
    {
        return cDayLengthArray[index];
    }

    return 0;
}

void EnvironmentService::HandleUpdate(const UpdateEvent& acEvent) noexcept
{
    // we apply the time here so we can apply it at a fixed rate
    //(the game does not do this)
    if (m_enableMPtime)
    {
        // update server time projection
        m_time = (acEvent.Delta * (m_timeScale * 0.00027777778f)) + m_time;
        if (m_time > 24.f)
        {
            int iDayLimit = GetNumerOfDaysByMonthIndex(m_month);

            while (m_time > 24.f)
            {
                m_time = m_time + -24.f;
                m_day++;
            }

            if (m_day > iDayLimit)
            {
                m_month++;
                m_day = m_day - iDayLimit;

                if (m_month > 12)
                {
                    m_month = m_month + -12;
                    m_year++;
                }
            }
        }

        // apply the time to the game
        auto *pGameTime = TimeData::Get();
        pGameTime->GameHour->f = m_time;
        pGameTime->GameDay->i = m_day;
        pGameTime->GameMonth->i = m_month;
        pGameTime->GameYear->i = m_year;
        pGameTime->GameDaysPassed->f = (m_time * 0.041666668f) + m_day;

        std::printf("time %f\n", m_time);
    }
}
