#pragma once

struct World;
struct ImguiService;
struct UpdateEvent;
struct MagicSyncEvent;

struct TransportService;

struct Actor;
struct TestService
{
    TestService(entt::dispatcher& aDispatcher, World& aWorld, TransportService& aTransport, ImguiService& aImguiService);
    ~TestService() noexcept;

    TP_NOCOPYMOVE(TestService);

    void OnUpdate(const UpdateEvent&) noexcept;
    #if TP_SKYRIM64
    void OnMagicSyncEvent(const MagicSyncEvent& acEvent) noexcept;
    #endif

protected:

    void ControlTestActor() noexcept;

    void OnDraw() noexcept;

private:

    void PlaceActorInWorld() noexcept;
    void RunDiff();

    entt::dispatcher& m_dispatcher;
    TransportService& m_transport;
    World& m_world;

    Vector<GamePtr<Actor>> m_actors;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_drawImGuiConnection;

};
