#pragma once

struct World;
struct ImguiService;
struct UpdateEvent;
struct MagicSyncEvent;
struct ActionEvent;

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
    void OnActionEvent(const ActionEvent& acEvent) noexcept;
    void ExecuteAction(const ActionEvent& acEvent) noexcept;
#endif

protected:

    void ControlTestActor() noexcept;
    void AnimationDebugging() noexcept;

    void OnDraw() noexcept;

private:

    void PlaceActorInWorld() noexcept;
    void RunDiff();

    entt::dispatcher& m_dispatcher;
    TransportService& m_transport;
    World& m_world;

    Vector<GamePtr<Actor>> m_actors;
    Vector<ActionEvent> m_actions;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_drawImGuiConnection;

};
