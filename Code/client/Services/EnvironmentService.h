#pragma once

#include <Structs/TimeModel.h>
#include <Events/EventDispatcher.h>
#include <Games/Events.h>

struct ImguiService;
struct ServerTimeSettings;
struct DisconnectedEvent;
struct World;
struct UpdateEvent;
struct ActivateEvent;

class EnvironmentService final 
    : public BSTEventSink<TESActivateEvent>
{
public:
    EnvironmentService(World&, entt::dispatcher&, ImguiService&);

    static bool AllowGameTick() noexcept;
  private:
    void OnTimeUpdate(const ServerTimeSettings &) noexcept;
    void HandleUpdate(const UpdateEvent &) noexcept;
    void OnDisconnected(const DisconnectedEvent &) noexcept;
    void OnDraw() noexcept;
    void OnActivate(const ActivateEvent&) noexcept;

    BSTEventResult OnEvent(const TESActivateEvent*, const EventDispatcher<TESActivateEvent>*) override;

    void ToggleGameClock(bool aEnable);
    float TimeInterpolate(const TimeModel& aFrom, TimeModel& aTo) const;

    void AddObjectComponent(TESObjectREFR* apObject) noexcept;

    entt::scoped_connection m_timeUpdateConnection;
    entt::scoped_connection m_weatherUpdateConnection;
    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_disconnectedConnection;
    entt::scoped_connection m_drawConnection;

    TimeModel m_onlineTime;
    TimeModel m_offlineTime;
    float m_fadeTimer = 0.f;
    bool m_switchToOffline = false;
    static bool s_gameClockLocked;

    uint64_t m_lastTick = 0;
    World& m_world;
};
