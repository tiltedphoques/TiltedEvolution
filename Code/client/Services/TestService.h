#pragma once

struct World;
struct ImguiService;
struct UpdateEvent;

struct TransportService;
struct BSAnimationGraphManager;
struct Actor;



struct GetVariablesEvent;
struct SetVariablesEvent;
struct SetStateEvent;

struct TestService
{
    TestService(entt::dispatcher& aDispatcher, World& aWorld, TransportService& aTransport, ImguiService& aImguiService);
    ~TestService() noexcept;

    TP_NOCOPYMOVE(TestService);

    void OnUpdate(const UpdateEvent&) noexcept;
    void OnGetVariables(const GetVariablesEvent&) noexcept;
    void OnSetVariables(const SetVariablesEvent&) noexcept;
    void OnSetState(const SetStateEvent&) noexcept;

protected:

    void AnimationDebugging() noexcept;

    void OnDraw() noexcept;

private:

    void PlaceActorInWorld() noexcept;
    void RunDiff();

    uint64_t DisplayGraphDescriptorKey(BSAnimationGraphManager* pManager) noexcept;

    entt::dispatcher& m_dispatcher;
    TransportService& m_transport;
    World& m_world;

    Vector<GamePtr<Actor>> m_actors;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_drawImGuiConnection;

};
