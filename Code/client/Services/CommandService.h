#pragma once

struct World;
struct TransportService;
struct CommandEvent;
struct TeleportCommandResponse;

struct CommandService
{
public:
    CommandService(World& aWorld, TransportService& aTransport, entt::dispatcher& aDispatcher);
    ~CommandService() noexcept = default;

    TP_NOCOPYMOVE(CommandService);

protected:
    void OnCommandEvent(const CommandEvent&) noexcept;
    void OnTeleportCommandResponse(const TeleportCommandResponse&) noexcept;

private:
    World& m_world;
    TransportService& m_transport;

    entt::scoped_connection m_commandEventConnection;
    entt::scoped_connection m_teleportConnection;
};
