#include <Services/CalendarService.h>

#include <World.h>
#include <Events/UpdateEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Messages/ServerTimeSettings.h>

#include <TimeManager.h>
#include <PlayerCharacter.h>
#include <Forms/TESObjectCELL.h>

constexpr float kTransitionSpeed = 5.f;

bool CalendarService::s_gameClockLocked = false;

bool CalendarService::AllowGameTick() noexcept
{
    return !s_gameClockLocked;
}

CalendarService::CalendarService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) 
    : m_world(aWorld)
    , m_transport(aTransport)
{
    m_timeUpdateConnection = aDispatcher.sink<ServerTimeSettings>().connect<&CalendarService::OnTimeUpdate>(this);
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&CalendarService::HandleUpdate>(this);
    m_disconnectedConnection = aDispatcher.sink<DisconnectedEvent>().connect<&CalendarService::OnDisconnected>(this);
}

void CalendarService::OnTimeUpdate(const ServerTimeSettings& acMessage) noexcept
{
    // disable the game clock
    m_onlineTime.TimeScale = acMessage.TimeScale;
    m_onlineTime.Time = acMessage.Time;
    ToggleGameClock(false);
}

void CalendarService::OnDisconnected(const DisconnectedEvent&) noexcept
{
    // signal a time transition
    m_fadeTimer = 0.f;
    m_switchToOffline = true;
}

float CalendarService::TimeInterpolate(const TimeModel& aFrom, TimeModel& aTo) const
{
    const auto t = aTo.Time - aFrom.Time;
    if (t < 0.f)
    {
        const auto v = t + 24.f;
        // interpolate on the time difference, not the time
        const auto x = TiltedPhoques::Lerp(0.f, v, m_fadeTimer / kTransitionSpeed) + aFrom.Time;

        return TiltedPhoques::Mod(x, 24.f);
    }
    
    return TiltedPhoques::Lerp(aFrom.Time, aTo.Time, m_fadeTimer / kTransitionSpeed);
}

void CalendarService::ToggleGameClock(bool aEnable)
{
    auto* pGameTime = TimeData::Get();
    if (aEnable)
    {
        pGameTime->GameDay->i = m_offlineTime.Day;
        pGameTime->GameMonth->i = m_offlineTime.Month;
        pGameTime->GameYear->i = m_offlineTime.Year;
        pGameTime->TimeScale->f = m_offlineTime.TimeScale;
        pGameTime->GameDaysPassed->f = (m_offlineTime.Time * (1.f / 24.f)) + m_offlineTime.Day;
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

    s_gameClockLocked = !aEnable;
}

void CalendarService::HandleUpdate(const UpdateEvent& aEvent) noexcept
{
    if (s_gameClockLocked)
    {
        const auto updateDelta = static_cast<float>(aEvent.Delta);
        auto* pGameTime = TimeData::Get();

        if (!m_lastTick)
            m_lastTick = m_world.GetTick();

        const auto now = m_world.GetTick();

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

        const auto delta = now - m_lastTick;
        m_lastTick = now;

        m_onlineTime.Update(delta);
        pGameTime->GameDay->i = m_onlineTime.Day;
        pGameTime->GameMonth->i = m_onlineTime.Month;
        pGameTime->GameYear->i = m_onlineTime.Year;
        pGameTime->TimeScale->f = m_onlineTime.TimeScale;
        pGameTime->GameDaysPassed->f = (m_onlineTime.Time * (1.f / 24.f)) + m_onlineTime.Day;

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
