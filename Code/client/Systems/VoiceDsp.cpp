#include "VoiceDsp.h"

#include <algorithm>
#include <cmath>

namespace VoiceDsp
{
float ClampMasterVolume(float aVolume) noexcept
{
    return std::isfinite(aVolume) ? std::clamp(aVolume, 0.f, 5.f) : 2.5f;
}

float Peak(const float* apSamples, size_t aCount) noexcept
{
    float peak = 0.f;
    for (size_t i = 0; i < aCount; ++i)
        peak = std::max(peak, std::fabs(apSamples[i]));

    return std::min(peak, 1.f);
}

float DistanceGain(float aDistance, float aFadeStart) noexcept
{
    if (!std::isfinite(aDistance) || !std::isfinite(aFadeStart) || aFadeStart <= 0.f)
        return 0.f;

    if (aDistance <= aFadeStart)
        return 1.f;

    const float silentAt = aFadeStart * 2.f;
    if (aDistance >= silentAt)
        return 0.f;

    return (silentAt - aDistance) / aFadeStart;
}

void PanGains(float aPan, float& aOutLeft, float& aOutRight) noexcept
{
    constexpr float kQuarterPi = 0.78539816f;

    const float pan = std::isfinite(aPan) ? std::clamp(aPan, -1.f, 1.f) : 0.f;
    const float angle = (pan + 1.f) * kQuarterPi;

    aOutLeft = std::cos(angle);
    aOutRight = std::sin(angle);
}

int32_t SequenceDelta(uint16_t aLast, uint16_t aNow) noexcept
{
    return static_cast<int16_t>(static_cast<uint16_t>(aNow - aLast));
}

bool NoiseGate::Process(float aPeak, float aThreshold) noexcept
{
    if (aPeak > 0.f && aPeak >= aThreshold)
        m_holdLeft = kHoldFrames;
    else if (m_holdLeft > 0)
        --m_holdLeft;

    return m_holdLeft > 0;
}

JitterBuffer::JitterBuffer()
    : m_samples(kCapacity, 0.f)
{
}

void JitterBuffer::Push(const float* apSamples, size_t aCount) noexcept
{
    for (size_t i = 0; i < aCount; ++i)
    {
        if (m_size == kCapacity)
        {
            m_read = (m_read + 1) % kCapacity;
            --m_size;
        }

        m_samples[(m_read + m_size) % kCapacity] = apSamples[i];
        ++m_size;
    }

    // Too far behind, skip ahead to the start threshold
    if (m_size > kMaxSamples)
    {
        const size_t drop = m_size - kStartSamples;
        m_read = (m_read + drop) % kCapacity;
        m_size -= drop;
    }

    if (!m_playing && m_size >= kStartSamples)
        m_playing = true;
}

size_t JitterBuffer::MixInto(float* apStereoOut, size_t aFrames, float aLeftGain, float aRightGain) noexcept
{
    if (!m_playing)
        return 0;

    const size_t count = std::min(aFrames, m_size);
    for (size_t i = 0; i < count; ++i)
    {
        const float sample = m_samples[(m_read + i) % kCapacity];
        apStereoOut[i * 2] += sample * aLeftGain;
        apStereoOut[i * 2 + 1] += sample * aRightGain;
    }

    m_read = (m_read + count) % kCapacity;
    m_size -= count;

    if (m_size == 0)
        m_playing = false;

    return count;
}

void JitterBuffer::Clear() noexcept
{
    m_read = 0;
    m_size = 0;
    m_playing = false;
}
} // namespace VoiceDsp
