#pragma once

#include <Events/AdminPacketEvent.h>
#include <spdlog/sinks/base_sink.h>

struct World;
struct UpdateEvent;
struct AdminShutdownRequest;

/**
* @brief Handles communication from an admin client.
* 
* This service is currently not in use.
*/
class AdminService : public spdlog::sinks::base_sink<spdlog::details::null_mutex>
{
public:
    AdminService(World& aWorld, entt::dispatcher& aDispatcher);

private:
    void HandleShutdown(const AdminPacketEvent<AdminShutdownRequest>& aChanges) noexcept;

    void sink_it_(const spdlog::details::log_msg& msg) override;
    void flush_() override;

    Vector<String> m_messages;
    entt::scoped_connection m_shutdownConnection;
    World& m_world;
};
