#pragma once

struct World;
struct TransportService;
struct CommandEvent;

struct CommandService
{
public:
    CommandService(World& aWorld, TransportService& transport, entt::dispatcher& aDispatcher);
    ~CommandService() noexcept = default;

    TP_NOCOPYMOVE(CommandService);

protected:
    void OnCommandEvent(const CommandEvent&) noexcept;

private:
    World& m_world;
    TransportService& m_transport;

    entt::scoped_connection m_commandEventConnection;
};
