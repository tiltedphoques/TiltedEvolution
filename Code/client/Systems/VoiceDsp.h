#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

/**
 * @brief Game independent pieces of voice chat: the noise gate, distance falloff, panning and the per speaker
 * jitter buffer. Kept apart from the audio devices and the game so they can be unit tested.
 *
 * Audio is 48 kHz, mono per speaker, in 20 ms frames of 960 samples.
 */
namespace VoiceDsp
{
constexpr uint32_t kSampleRate = 48000;
constexpr uint32_t kFrameSamples = 960;

// Playback multiplier: 0 mutes, 1 is unchanged, 5 is the maximum. Invalid values use 100%.
float ClampMasterVolume(float aVolume) noexcept;

// Peak of a frame, 0 to 1. This is what the Windows "Test your microphone" bar shows.
float Peak(const float* apSamples, size_t aCount) noexcept;

// Full volume up to aFadeStart, then linear down to silent at twice that distance
float DistanceGain(float aDistance, float aFadeStart) noexcept;

// aPan is -1 (all left) to 1 (all right), constant power so a voice moving around doesn't change loudness
void PanGains(float aPan, float& aOutLeft, float& aOutRight) noexcept;

// Frames between the last sequence and this one, wrapping at 65536. 1 is the next frame, 0 or less is a duplicate or late.
int32_t SequenceDelta(uint16_t aLast, uint16_t aNow) noexcept;

/**
 * @brief Opens on a frame whose peak reaches the threshold, and stays open a moment after so word endings and short
 * pauses aren't cut off. A threshold of 0 is always open.
 */
struct NoiseGate
{
    // 300 ms
    static constexpr uint32_t kHoldFrames = 15;

    // Returns whether this frame should be sent
    bool Process(float aPeak, float aThreshold) noexcept;

    [[nodiscard]] bool IsOpen() const noexcept { return m_holdLeft > 0; }

private:
    uint32_t m_holdLeft{0};
};

/**
 * @brief Decoded audio of one remote speaker, waiting to be played.
 *
 * Playback starts once kStartSamples are buffered so small network jitter doesn't cause gaps. When the buffer
 * runs dry the speaker goes quiet and buffers up again. If it grows past kMaxSamples, e.g. after our game froze,
 * the oldest audio is dropped so the voice doesn't lag behind.
 */
struct JitterBuffer
{
    static constexpr size_t kStartSamples = kFrameSamples * 3; // 60 ms
    static constexpr size_t kMaxSamples = kFrameSamples * 10;  // 200 ms
    static constexpr size_t kCapacity = kFrameSamples * 16;

    JitterBuffer();

    void Push(const float* apSamples, size_t aCount) noexcept;
    // Adds up to aFrames samples into the interleaved stereo output, returns how many were mixed
    size_t MixInto(float* apStereoOut, size_t aFrames, float aLeftGain, float aRightGain) noexcept;
    void Clear() noexcept;

    [[nodiscard]] size_t GetBuffered() const noexcept { return m_size; }
    [[nodiscard]] bool IsPlaying() const noexcept { return m_playing; }

private:
    std::vector<float> m_samples;
    size_t m_read{0};
    size_t m_size{0};
    bool m_playing{false};
};
} // namespace VoiceDsp
