#pragma once

#include <Events/PacketEvent.h>

struct World;
struct Player;
struct TeleportCommandRequest;
struct SetTimeCommandRequest;
struct RequestTimeSkip;

/**
 * @brief Processes incoming commands.
 */
struct CommandService
{
    CommandService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~CommandService() noexcept = default;

    TP_NOCOPYMOVE(CommandService);

protected:
    void OnSetTimeCommand(const PacketEvent<SetTimeCommandRequest>& acMessage) const noexcept;
    /**
     * @brief Advances the shared time after a player slept or waited (#467). Same permissions as /settime.
     */
    void OnTimeSkipRequest(const PacketEvent<RequestTimeSkip>& acMessage) const noexcept;
    bool CanChangeTime(const Player* apPlayer) const noexcept;
    /**
     * @brief Returns the location of the target player of the teleport command.
     */
    void OnTeleportCommandRequest(const PacketEvent<TeleportCommandRequest>& acMessage) const noexcept;

private:
    World& m_world;

    entt::scoped_connection m_setTimeConnection;
    entt::scoped_connection m_timeSkipConnection;
    entt::scoped_connection m_teleportConnection;
};
