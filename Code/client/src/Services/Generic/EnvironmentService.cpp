
#include <World.h>
#include <Events/UpdateEvent.h>
#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Services/EnvironmentService.h>
#include <Messages/ServerTimeSettings.h>

#include <Games/Skyrim/TimeManager.h>

constexpr float kTransitionSpeed = 5.f;

bool EnvironmentService::s_gameClockLocked = false;

bool EnvironmentService::AllowGameTick()
{
    return !s_gameClockLocked;
}

EnvironmentService::EnvironmentService(World& aWorld, entt::dispatcher& aDispatcher) : m_world(aWorld)
{
    m_timeUpdateConnection = aDispatcher.sink<ServerTimeSettings>().connect<&EnvironmentService::OnTimeUpdate>(this);
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&EnvironmentService::HandleUpdate>(this);
    m_disconnectedConnection = aDispatcher.sink<DisconnectedEvent>().connect<&EnvironmentService::OnDisconnected>(this);
}

void EnvironmentService::OnTimeUpdate(const ServerTimeSettings &acMessage)
{
    // disable the game clock
    m_onlineTime.TimeScale = acMessage.TimeScale;
    m_onlineTime.Time = acMessage.Time;
    ToggleGameClock(false);
}

void EnvironmentService::OnDisconnected(const DisconnectedEvent &) noexcept
{
    // signal a time transition
    m_fadeTimer = 0.f;
    m_switchToOffline = true;
}

float EnvironmentService::TimeInterpolate(const TimeModel& from, TimeModel& to)
{
    float t = to.Time - from.Time;
    if (t < 0.f)
    {
        float v = t + 24.f;
        // interpolate on the time difference, not the time
        float x = TiltedPhoques::Lerp(0.f, v, m_fadeTimer / kTransitionSpeed) + from.Time;
        if (x > 24.f)
            x = x - 24.f;
        return x;
    }
    else
        return TiltedPhoques::Lerp(from.Time, to.Time, m_fadeTimer / kTransitionSpeed);
}

void EnvironmentService::ToggleGameClock(bool enable)
{
    auto* pGameTime = TimeData::Get();
    if (enable)
    {
        pGameTime->GameDay->i = m_offlineTime.Day;
        pGameTime->GameMonth->i = m_offlineTime.Month;
        pGameTime->GameYear->i = m_offlineTime.Year;
        pGameTime->TimeScale->f = m_offlineTime.TimeScale;
        pGameTime->GameDaysPassed->f = (m_offlineTime.Time * 0.041666668f) + m_offlineTime.Day;
        pGameTime->GameHour->f = m_offlineTime.Time;
        m_switchToOffline = false;
    }
    else
    {
        m_offlineTime.Day = pGameTime->GameDay->i;
        m_offlineTime.Month = pGameTime->GameMonth->i;
        m_offlineTime.Year = pGameTime->GameYear->i;
        m_offlineTime.Time = pGameTime->GameHour->f;
        m_offlineTime.TimeScale = pGameTime->TimeScale->f;
    }

    s_gameClockLocked = !enable;
}

void EnvironmentService::HandleUpdate(const UpdateEvent& aEvent) noexcept
{
    if (s_gameClockLocked)
    {
        float updateDelta = static_cast<float>(aEvent.Delta);
        auto* pGameTime = TimeData::Get();

        if (!m_lastTick)
            m_lastTick = m_world.GetTick();

        auto now = m_world.GetTick();

        if (m_switchToOffline)
        {
            // time transition out
            if (m_fadeTimer < kTransitionSpeed)
            {
                pGameTime->GameHour->f = TimeInterpolate(m_onlineTime, m_offlineTime);
                // before we quit here we fire this event
                if ((m_fadeTimer + updateDelta) > kTransitionSpeed)
                {
                    m_fadeTimer += updateDelta;
                    ToggleGameClock(true);
                }
                else
                    m_fadeTimer += updateDelta;
            }
        }

        // we got disconnected or the client got ahead of us
        if (now < m_lastTick)
            return;
    
        auto delta = now - m_lastTick;
        m_lastTick = now;

        m_onlineTime.Update(delta);
        pGameTime->GameDay->i = m_onlineTime.Day;
        pGameTime->GameMonth->i = m_onlineTime.Month;
        pGameTime->GameYear->i = m_onlineTime.Year;
        pGameTime->TimeScale->f = m_onlineTime.TimeScale;
        pGameTime->GameDaysPassed->f = (m_onlineTime.Time * 0.041666668f) + m_onlineTime.Day;

        // time transition in
        if (m_fadeTimer < kTransitionSpeed)
        {
            pGameTime->GameHour->f = TimeInterpolate(m_offlineTime, m_onlineTime);
            m_fadeTimer += updateDelta;
        }
        else
            pGameTime->GameHour->f = m_onlineTime.Time;
    }
}
