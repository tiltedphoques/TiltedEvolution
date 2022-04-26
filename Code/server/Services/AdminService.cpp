#include <GameServer.h>

#include <World.h>
#include <Services/AdminService.h>

#include <AdminMessages/AdminShutdownRequest.h>
#include <AdminMessages/ServerLogs.h>


AdminService::AdminService(World& aWorld, entt::dispatcher& aDispatcher) : m_world(aWorld)
{
    m_shutdownConnection =
        aDispatcher.sink<AdminPacketEvent<AdminShutdownRequest>>().connect<&AdminService::HandleShutdown>(this);
}

void AdminService::HandleShutdown(const AdminPacketEvent<AdminShutdownRequest>& acMessage) noexcept
{
    spdlog::warn("Shutdown was requested by {:x}", acMessage.ConnectionId);

    GameServer::Get()->Kill();
}

void AdminService::sink_it_(const spdlog::details::log_msg& msg)
{
    spdlog::memory_buf_t formatted;
    formatter_->format(msg, formatted);
    
    ServerLogs logs;
    logs.Logs = fmt::to_string(formatted);

    GameServer::Get()->ForEachAdmin([&logs](ConnectionId_t aId) { GameServer::Get()->Send(aId, logs); });
}

void AdminService::flush_()
{
}
