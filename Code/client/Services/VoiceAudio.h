#pragma once

#include <Structs/VoiceFrame.h>

/**
 * @brief The audio side of voice chat: microphone capture with the noise gate and Opus encoding, and decoding and
 * mixing of remote voices into an output device. Uses miniaudio for the devices and libopus for the codec.
 *
 * Capture and playback run on miniaudio's device threads. Everything else is called from the game's main thread,
 * the two sides only share small locked queues and atomics.
 */
struct VoiceAudio
{
    struct DeviceLists
    {
        Vector<String> Inputs;
        Vector<String> Outputs;
    };

    VoiceAudio() noexcept;
    ~VoiceAudio() noexcept;

    TP_NOCOPYMOVE(VoiceAudio);

    // Device names as Windows shows them, for the Settings menu
    bool ListDevices(DeviceLists& aOutLists) noexcept;

    // An empty input name captures nothing, "default" follows the Windows default device. An empty output name
    // plays on the Windows default device, which is the one the game uses too. Restarts devices that changed.
    void Start(const String& acInputDevice, const String& acOutputDevice) noexcept;
    void Stop() noexcept;
    [[nodiscard]] bool IsRunning() const noexcept;

    // Volume 0 to 1 applied before the gate and the meter, gate threshold 0 to 1 of peak level
    void SetCaptureSettings(float aMicVolume, float aNoiseGate) noexcept;
    // While false the microphone is still read for the meter, but nothing is encoded or sent
    void SetTransmitting(bool aTransmitting) noexcept;
    // Peak level of the last microphone frame after volume, 0 to 1, for the Settings meter
    [[nodiscard]] float GetMicLevel() const noexcept;
    // Encoded frames waiting to be sent, oldest first
    bool PopEncodedFrame(VoiceFrame& aOutFrame) noexcept;

    // Decodes a remote player's frame into their jitter buffer
    void PushRemoteFrame(uint32_t aPlayerId, const VoiceFrame& acFrame) noexcept;
    // Stereo gains for a remote player, 0 mutes them without dropping their audio
    void SetSpeakerGains(uint32_t aPlayerId, float aLeft, float aRight) noexcept;
    void RemoveSpeaker(uint32_t aPlayerId) noexcept;
    void RemoveAllSpeakers() noexcept;
    // Players whose voice arrived in the last few hundred ms, whether or not they can be heard
    void GetActiveSpeakers(Vector<uint32_t>& aOutPlayerIds) const noexcept;

private:
    struct Impl;
    UniquePtr<Impl> m_pImpl;
};
