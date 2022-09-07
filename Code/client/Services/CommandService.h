#pragma once

struct World;
struct TransportService;
struct TeleportCommandResponse;

/**
* @brief Responsible for processing, sending and receiving chat commands.
* 
* Parses and sends appropriate commands to the server,
* and receives and processes command results processed by the server.
*/
struct CommandService
{
public:
    CommandService(World& aWorld, TransportService& aTransport, entt::dispatcher& aDispatcher);
    ~CommandService() noexcept = default;

    TP_NOCOPYMOVE(CommandService);

protected:
    /**
    * @brief Processes result of teleport command.
    */
    void OnTeleportCommandResponse(const TeleportCommandResponse&) noexcept;

private:
    World& m_world;
    TransportService& m_transport;

    entt::scoped_connection m_teleportConnection;
};
