#pragma once

#include <Services/VoiceAudio.h>

struct World;
struct TransportService;
struct UpdateEvent;
struct ConnectedEvent;
struct DisconnectedEvent;
struct NotifyVoiceData;
struct NotifyPlayerLeft;

/**
 * @brief Built-in proximity voice chat.
 *
 * The microphone is captured, gated and Opus encoded by VoiceAudio, sent to the server over the game connection
 * and relayed to players nearby. Remote voices get louder or quieter with distance from the camera: full volume up to
 * the fade start, silent at twice that. Nothing else is needed, no separate program and no extra port.
 *
 * Voice runs while the server's Voice:bVoip is on and the player ticked Connect to voip, which is the same key in
 * the player's own Data/SkyrimTogetherReborn/config/STServer.ini. The other [Voice] keys there are the player's
 * Settings menu choices. Per player volumes live in the UI, which pushes them here by player id.
 */
struct VoiceService
{
    enum class Channel : uint8_t
    {
        kEveryone = 0,
        kParty,
        kStrangers
    };

    struct Settings
    {
        // Empty captures nothing, "default" follows the Windows default microphone
        String InputDevice{};
        bool Connect{false};
        // Empty plays on the Windows default device, the one the game uses
        String OutputDevice{};
        Channel ListenChannel{Channel::kEveryone};
        float FadeStart{2500.f};
        float MicVolume{1.f};
        float NoiseGate{0.1f};
        float MasterVolume{2.5f};
    };

    VoiceService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept;
    ~VoiceService() noexcept;

    TP_NOCOPYMOVE(VoiceService);

    // Called from the UI, on the main thread
    void RefreshUi() noexcept;
    void ApplySettings(const Settings& acSettings) noexcept;
    void SetPlayerVolume(uint32_t aPlayerId, float aVolume) noexcept;
    void SetMeterEnabled(bool aEnabled) noexcept;

    [[nodiscard]] const Settings& GetSettings() const noexcept { return m_settings; }

protected:
    void OnUpdate(const UpdateEvent& acEvent) noexcept;
    void OnConnected(const ConnectedEvent& acEvent) noexcept;
    void OnDisconnected(const DisconnectedEvent& acEvent) noexcept;
    void OnVoiceData(const NotifyVoiceData& acMessage) noexcept;
    void OnPlayerLeft(const NotifyPlayerLeft& acMessage) noexcept;

private:
    [[nodiscard]] bool IsVoiceWanted() const noexcept;
    void UpdateAudioState() noexcept;
    void SendListeningState() noexcept;
    void SendOutgoingFrames() noexcept;
    void UpdateSpeakerGains() noexcept;
    void UpdateUi(double aDelta) noexcept;

    void LoadSettings() noexcept;
    void SaveSettings() const noexcept;
    void SendSettingsToUi() const noexcept;

    World& m_world;
    TransportService& m_transport;
    VoiceAudio m_audio;

    Settings m_settings{};
    Map<uint32_t, float> m_playerVolumes;

    // What the server was last told, so it's only sent on change
    bool m_listeningSent{false};
    bool m_sentListening{false};
    Channel m_sentChannel{Channel::kEveryone};

    bool m_meterEnabled{false};
    double m_uiTimer{0.0};
    Vector<uint32_t> m_lastTalking;

    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_connectedConnection;
    entt::scoped_connection m_disconnectedConnection;
    entt::scoped_connection m_voiceDataConnection;
    entt::scoped_connection m_playerLeftConnection;
};
