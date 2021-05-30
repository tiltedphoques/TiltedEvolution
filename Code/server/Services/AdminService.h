#pragma once

#include <Events/AdminPacketEvent.h>
#include <Structs/GameId.h>

struct World;
struct UpdateEvent;
struct AdminShutdownRequest;

class AdminService
{
public:
    AdminService(World& aWorld, entt::dispatcher& aDispatcher);

private:
    void HandleShutdown(const AdminPacketEvent<AdminShutdownRequest>& aChanges) noexcept;

    entt::scoped_connection m_shutdownConnection;
    World& m_world;
};
