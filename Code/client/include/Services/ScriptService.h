#pragma once

#include <ScriptStore.h>
#include <Structs/Scripts.h>
#include <Structs/FullObjects.h>
#include <Structs/Objects.h>

struct RemoteComponent;
struct TransportService;
struct DisconnectedEvent;
struct ConnectedEvent;
struct LocalComponent;
struct FormIdComponent;
struct World;
struct UpdateEvent;
struct ImguiService;

struct ScriptService : ScriptStore
{
    ScriptService(World& aWorld, entt::dispatcher& aDispatcher, ImguiService& aImguiService, TransportService& aTransportService) noexcept;
    ~ScriptService() noexcept;

    TP_NOCOPYMOVE(ScriptService);

    void RegisterExtensions(ScriptContext& aContext) override;

protected:

    void OnUpdate(const UpdateEvent&) noexcept;
    void OnDraw() noexcept;
    void OnScripts(const Scripts& acScripts) noexcept;
    void OnNetObjectsInitalize(const FullObjects& acNetObjects) noexcept;
    void OnNetObjectsUpdate(const Objects& acNetObjects) noexcept;
    void OnConnected(const ConnectedEvent&) noexcept;
    void OnDisconnected(const DisconnectedEvent&) noexcept;
    void DisplayNetObject(NetObject* apObject);

private:

    void DisplayNetObjects() noexcept;
    void DisplayEntities() noexcept;
    void DisplayEntityPanel(entt::entity aEntity) noexcept;
    void DisplayFormComponent(FormIdComponent& aFormComponent) const noexcept;
    void DisplayLocalComponent(LocalComponent& aLocalComponent) const noexcept;
    void DisplayRemoteComponent(RemoteComponent& aLocalComponent) const noexcept;

    entt::dispatcher& m_dispatcher;
    World& m_world;
    TransportService& m_transport;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_scriptsConnection;
    entt::scoped_connection m_replicatedInitConnection;
    entt::scoped_connection m_replicatedUpdateConnection;
    entt::scoped_connection m_drawImGuiConnection;

    entt::scoped_connection m_connectedConnection;
    entt::scoped_connection m_disconnectedConnection;
};
