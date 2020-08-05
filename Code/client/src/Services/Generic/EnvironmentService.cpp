
#include <Events/UpdateEvent.h>
#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Services/EnvironmentService.h>
#include <Messages/ServerTimeSettings.h>

#include <Games/Skyrim/TimeManager.h>

EnvironmentService::EnvironmentService(entt::dispatcher &aDispatcher)
{
    m_TimeUpdateConnection = aDispatcher.sink<ServerTimeSettings>().connect<&EnvironmentService::OnTimeUpdate>(this);
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&EnvironmentService::HandleUpdate>(this);
    m_connectedConnection = aDispatcher.sink<ConnectedEvent>().connect<&EnvironmentService::OnConnected>(this);
    m_disconnectedConnection = aDispatcher.sink<DisconnectedEvent>().connect<&EnvironmentService::OnDisconnected>(this);
}

void EnvironmentService::OnTimeUpdate(const ServerTimeSettings &acMessage)
{
    std::printf("i got the time!!!!");
    m_fTimeScale = acMessage.TimeScale;
    m_fTime = acMessage.Time;
}

void EnvironmentService::OnConnected(const ConnectedEvent &acConnectedEvent) const noexcept
{
    // TBD
}

void EnvironmentService::OnDisconnected(const DisconnectedEvent &acDisconnectedEvent) noexcept
{
    m_fTime = -1.f;
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
    if (m_fTime != -1.f)
    {
        // update server time projection
        m_fTime = (acEvent.Delta * (m_fTimeScale * 0.00027777778f)) + m_fTime;
        if (m_fTime > 24.f)
        {
            int iDayLimit = GetNumerOfDaysByMonthIndex(m_iMonth);

            while (m_fTime > 24.f)
            {
                m_fTime = m_fTime + -24.f;
                m_iDay++;
            }

            if (m_iDay > iDayLimit)
            {
                m_iMonth++;
                m_iDay = m_iDay - iDayLimit;

                if (m_iMonth > 12)
                {
                    m_iMonth = m_iMonth + -12;
                    m_iYear++;
                }
            }
        }

        // apply the time to the game
        auto *pGameTime = TimeData::Get();
        pGameTime->GameHour->f = m_fTime;
        pGameTime->GameDay->i = m_iDay;
        pGameTime->GameMonth->i = m_iMonth;
        pGameTime->GameYear->i = m_iYear;
        pGameTime->GameDaysPassed->f = (m_fTime * 0.041666668f) + m_iDay;

        std::printf("time %f\n", m_fTime);
    }
}
