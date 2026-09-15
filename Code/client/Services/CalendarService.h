#pragma once

#include <DateTime.h>
#include <Events/EventDispatcher.h>
#include <Games/Events.h>

struct ServerTimeSettings;
struct DisconnectedEvent;
struct World;
struct UpdateEvent;
struct TransportService;

/**
 * @brief Handles time sync.
 */
class CalendarService final : public BSTEventSink<TESSleepStartEvent>, public BSTEventSink<TESSleepStopEvent>, public BSTEventSink<TESWaitStartEvent>, public BSTEventSink<TESWaitStopEvent>
{
public:
    CalendarService(World&, entt::dispatcher&, TransportService&);
    ~CalendarService() override;

    static bool AllowGameTick() noexcept;

    // Sleeping and waiting are turned into a shared time skip (#467).
    BSTEventResult OnEvent(const TESSleepStartEvent* apEvent, const EventDispatcher<TESSleepStartEvent>*) override;
    BSTEventResult OnEvent(const TESSleepStopEvent* apEvent, const EventDispatcher<TESSleepStopEvent>*) override;
    BSTEventResult OnEvent(const TESWaitStartEvent* apEvent, const EventDispatcher<TESWaitStartEvent>*) override;
    BSTEventResult OnEvent(const TESWaitStopEvent* apEvent, const EventDispatcher<TESWaitStopEvent>*) override;

private:
    void BeginTimeSkip(float aStartTimeInDays, float aDesiredEndTimeInDays, const char* apKind) noexcept;
    void EndTimeSkip(bool aInterrupted, const char* apKind) noexcept;

    void OnTimeUpdate(const ServerTimeSettings&) noexcept;
    void HandleUpdate(const UpdateEvent&) noexcept;
    void OnDisconnected(const DisconnectedEvent&) noexcept;

    void ToggleGameClock(bool aEnable);
    float TimeInterpolate(const TimeModel& aFrom, TimeModel& aTo) const;

    entt::scoped_connection m_timeUpdateConnection;
    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_disconnectedConnection;

    DateTime m_onlineTime;
    DateTime m_offlineTime;
    // Mirror of the server clock (date included, whatever bSyncPlayerCalendar says) to measure time jumps.
    DateTime m_serverTime;
    bool m_hasServerTime = false;
    float m_fadeTimer = 0.f;
    static bool s_gameClockLocked;

    uint64_t m_lastTick = 0;
    uint64_t m_lastLogTick = 0;
    float m_pendingSkipHours = 0.f;
    World& m_world;
    TransportService& m_transport;
};
