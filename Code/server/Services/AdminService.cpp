#include <stdafx.h>

#include <GameServer.h>

#include <World.h>
#include <Services/AdminService.h>

#include <AdminMessages/AdminShutdownRequest.h>


AdminService::AdminService(World& aWorld, entt::dispatcher& aDispatcher) : m_world(aWorld)
{
    m_shutdownConnection =
        aDispatcher.sink<AdminPacketEvent<AdminShutdownRequest>>().connect<&AdminService::HandleShutdown>(this);
}

void AdminService::HandleShutdown(const AdminPacketEvent<AdminShutdownRequest>& acMessage) noexcept
{
    spdlog::warn("Shutdown was requested by {:x}", acMessage.ConnectionId);

    GameServer::Get()->Stop();
}
