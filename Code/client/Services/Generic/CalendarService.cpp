#include <Services/CalendarService.h>

#include <Events/DisconnectedEvent.h>
#include <Events/UpdateEvent.h>
#include <Messages/ServerTimeSettings.h>
#include <Messages/RequestTimeSkip.h>
#include <Services/TransportService.h>
#include <World.h>

#include <Forms/TESObjectCELL.h>
#include <PlayerCharacter.h>
#include <TimeManager.h>

constexpr float kTransitionSpeed = 5.f;

bool CalendarService::s_gameClockLocked = false;

bool CalendarService::AllowGameTick() noexcept
{
    return !s_gameClockLocked;
}

CalendarService::CalendarService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport)
    : m_world(aWorld), m_transport(aTransport)
{
    m_timeUpdateConnection = aDispatcher.sink<ServerTimeSettings>().connect<&CalendarService::OnTimeUpdate>(this);
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&CalendarService::HandleUpdate>(this);
    m_disconnectedConnection = aDispatcher.sink<DisconnectedEvent>().connect<&CalendarService::OnDisconnected>(this);

    auto* pEventList = EventDispatcherManager::Get();
    pEventList->sleepStartEvent.RegisterSink(this);
    pEventList->sleepStopEvent.RegisterSink(this);
    pEventList->waitStartEvent.RegisterSink(this);
    pEventList->waitStopEvent.RegisterSink(this);
}

CalendarService::~CalendarService()
{
    auto* pEventList = EventDispatcherManager::Get();
    pEventList->sleepStartEvent.UnRegisterSink(this);
    pEventList->sleepStopEvent.UnRegisterSink(this);
    pEventList->waitStartEvent.UnRegisterSink(this);
    pEventList->waitStopEvent.UnRegisterSink(this);
}

BSTEventResult CalendarService::OnEvent(const TESSleepStartEvent* apEvent, const EventDispatcher<TESSleepStartEvent>*)
{
    if (apEvent)
        BeginTimeSkip(apEvent->sleepStartTime, apEvent->desiredSleepEndTime, "sleep");
    return BSTEventResult::kOk;
}

BSTEventResult CalendarService::OnEvent(const TESSleepStopEvent* apEvent, const EventDispatcher<TESSleepStopEvent>*)
{
    EndTimeSkip(apEvent && apEvent->interrupted, "sleep");
    return BSTEventResult::kOk;
}

BSTEventResult CalendarService::OnEvent(const TESWaitStartEvent* apEvent, const EventDispatcher<TESWaitStartEvent>*)
{
    if (apEvent)
        BeginTimeSkip(apEvent->waitStartTime, apEvent->desiredWaitEndTime, "wait");
    return BSTEventResult::kOk;
}

BSTEventResult CalendarService::OnEvent(const TESWaitStopEvent* apEvent, const EventDispatcher<TESWaitStopEvent>*)
{
    EndTimeSkip(apEvent && apEvent->interrupted, "wait");
    return BSTEventResult::kOk;
}

void CalendarService::BeginTimeSkip(float aStartTimeInDays, float aDesiredEndTimeInDays, const char* apKind) noexcept
{
    m_pendingSkipHours = (aDesiredEndTimeInDays - aStartTimeInDays) * 24.f;

    spdlog::info("{} started: game time {} -> {} days, {} hours requested, online {}", apKind, aStartTimeInDays, aDesiredEndTimeInDays, m_pendingSkipHours, s_gameClockLocked);
}

void CalendarService::EndTimeSkip(bool aInterrupted, const char* apKind) noexcept
{
    const float hours = m_pendingSkipHours;
    m_pendingSkipHours = 0.f;

    // Offline the game advanced its own clock; online the clock is driven by the server, so ask it to skip.
    if (!s_gameClockLocked || !m_transport.IsConnected())
        return;

    if (aInterrupted || !(hours > 0.f) || hours > 48.f)
    {
        spdlog::info("{} ended without a time skip: interrupted {}, hours {}", apKind, aInterrupted, hours);
        return;
    }

    RequestTimeSkip request{};
    request.Hours = hours;
    m_transport.Send(request);

    spdlog::info("{} ended: requested a shared time skip of {} hours", apKind, hours);
}

void CalendarService::OnTimeUpdate(const ServerTimeSettings& acMessage) noexcept
{
    // A jump of the shared clock (sleep, wait, /settime) must also move GameDaysPassed, which drives script
    // timers, the way the game itself does when the player sleeps. Regular ticks are handled in HandleUpdate.
    // The comparison uses a mirror of the server clock, not m_onlineTime: with bSyncPlayerCalendar off the
    // online date is the local one and a midnight crossed on the server would count as a whole extra day.
    const DateTime newServerTime{acMessage.timeModel};
    if (s_gameClockLocked && m_hasServerTime)
    {
        const float skippedDays = newServerTime.GetTimeInDays() - m_serverTime.GetTimeInDays();
        if (skippedDays > 0.f && skippedDays < 60.f)
        {
            TimeData::Get()->GameDaysPassed->f += skippedDays;
            spdlog::info("Shared time jumped forward by {} days", skippedDays);
        }
    }
    m_serverTime = newServerTime;
    m_hasServerTime = true;

    // disable the game clock
    ToggleGameClock(false);
    m_onlineTime.m_timeModel.TimeScale = acMessage.timeModel.TimeScale;
    m_onlineTime.m_timeModel.Time = acMessage.timeModel.Time;

    if (m_world.GetServerSettings().SyncPlayerCalendar)
    {
        m_onlineTime.m_timeModel.Day = acMessage.timeModel.Day;
        m_onlineTime.m_timeModel.Month = acMessage.timeModel.Month;
        m_onlineTime.m_timeModel.Year = acMessage.timeModel.Year;
    }
    else
    {
        m_onlineTime.m_timeModel.Day = m_offlineTime.m_timeModel.Day;
        m_onlineTime.m_timeModel.Month = m_offlineTime.m_timeModel.Month;
        m_onlineTime.m_timeModel.Year = m_offlineTime.m_timeModel.Year;
    }
}

void CalendarService::OnDisconnected(const DisconnectedEvent&) noexcept
{
    m_hasServerTime = false;
    m_pendingSkipHours = 0.f;
    // signal a time transition
    m_fadeTimer = 0.f;
    ToggleGameClock(true);
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
    m_offlineTime.m_timeModel.Day = pGameTime->GameDay->f;
    m_offlineTime.m_timeModel.Month = pGameTime->GameMonth->f;
    m_offlineTime.m_timeModel.Year = pGameTime->GameYear->f;
    m_offlineTime.m_timeModel.Time = pGameTime->GameHour->f;
    m_offlineTime.m_timeModel.TimeScale = pGameTime->TimeScale->f;

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

        // we got disconnected or the client got ahead of us
        if (now < m_lastTick)
            return;

        const auto delta = now - m_lastTick;
        m_lastTick = now;

        m_onlineTime.Update(delta);
        m_serverTime.Update(delta);
        pGameTime->TimeScale->f = m_onlineTime.m_timeModel.TimeScale;
        pGameTime->GameDay->f = m_onlineTime.m_timeModel.Day;
        pGameTime->GameMonth->f = m_onlineTime.m_timeModel.Month;
        pGameTime->GameYear->f = m_onlineTime.m_timeModel.Year;
        pGameTime->GameDaysPassed->f += m_onlineTime.GetDeltaTime(delta) * (1.f / 24.f);

        // time transition in
        if (m_fadeTimer < kTransitionSpeed)
        {
            pGameTime->GameHour->f = TimeInterpolate(m_offlineTime.m_timeModel, m_onlineTime.m_timeModel);
            m_fadeTimer += updateDelta;
        }
        else
            pGameTime->GameHour->f = m_onlineTime.m_timeModel.Time;
    }
}
