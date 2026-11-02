#pragma once

#include <Events/PacketEvent.h>

struct World;
struct Player;
struct PlayerLeaveEvent;
struct VoiceDataRequest;
struct VoiceStateRequest;

/**
 * @brief Relays proximity voice chat between players.
 *
 * Voice frames are Opus packets the clients encode and decode themselves, the server only decides who gets them:
 * players in range of the speaker who have voip on and want that speaker's channel. Distance falloff is done by
 * each listener. Enabled with Voice:bVoip in STServer.ini.
 */
struct VoiceService
{
    VoiceService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~VoiceService() noexcept = default;

    TP_NOCOPYMOVE(VoiceService);

protected:
    void OnVoiceDataRequest(const PacketEvent<VoiceDataRequest>& acMessage) noexcept;
    void OnVoiceStateRequest(const PacketEvent<VoiceStateRequest>& acMessage) noexcept;
    void OnPlayerLeave(const PlayerLeaveEvent& acEvent) noexcept;

private:
    struct Listener
    {
        bool Listening{false};
        uint8_t Channel{0};
    };

    struct Speaker
    {
        std::chrono::steady_clock::time_point WindowStart{};
        uint32_t FramesInWindow{0};
    };

    bool IsWithinRateLimit(uint32_t aPlayerId) noexcept;

    World& m_world;

    // Keyed by player id
    Map<uint32_t, Listener> m_listeners;
    Map<uint32_t, Speaker> m_speakers;

    entt::scoped_connection m_voiceDataConnection;
    entt::scoped_connection m_voiceStateConnection;
    entt::scoped_connection m_playerLeaveConnection;
};
