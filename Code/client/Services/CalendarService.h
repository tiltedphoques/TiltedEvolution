#pragma once

#include <DateTime.h>
#include <Events/EventDispatcher.h>
#include <Games/Events.h>

struct ServerTimeSettings;
struct NotifyTimeSkip;
struct TimeSkipRequestEvent;
struct DisconnectedEvent;
struct World;
struct UpdateEvent;
struct TransportService;

/**
 * @brief Handles time sync.
 */
class CalendarService final : public BSTEventSink<TESActivateEvent>
{
public:
    CalendarService(World&, entt::dispatcher&, TransportService&);

    static bool AllowGameTick() noexcept;

private:
    void OnTimeUpdate(const ServerTimeSettings&) noexcept;
    void OnTimeSkipRequest(const TimeSkipRequestEvent&) noexcept;
    void OnNotifyTimeSkip(const NotifyTimeSkip&) noexcept;
    void HandleUpdate(const UpdateEvent&) noexcept;
    void OnDisconnected(const DisconnectedEvent&) noexcept;

    void ToggleGameClock(bool aEnable);
    float TimeInterpolate(const TimeModel& aFrom, TimeModel& aTo) const;

    entt::scoped_connection m_timeUpdateConnection;
    entt::scoped_connection m_timeSkipRequestConnection;
    entt::scoped_connection m_notifyTimeSkipConnection;
    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_disconnectedConnection;

    DateTime m_onlineTime;
    DateTime m_offlineTime;
    float m_fadeTimer = 0.f;
    float m_timeSkipEndDays = 0.f;
    bool m_timeSkipPending = false;
    static bool s_gameClockLocked;

    uint64_t m_lastTick = 0;
    uint64_t m_lastLogTick = 0;
    World& m_world;
    TransportService& m_transport;
};
