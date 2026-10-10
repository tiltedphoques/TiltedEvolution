#include "PlaybackClock.h"

#include <algorithm>
#include <cmath>
#include <limits>

bool PlaybackClock::AddSample(const double aArrivalTime, const uint64_t aCaptureTick) noexcept
{
    // A duplicate carries no new timing information and would skew the interval estimate
    for (size_t i = 0; i < m_sampleCount; ++i)
    {
        if (m_samples[i].CaptureTick == aCaptureTick)
            return false;
    }

    const bool cIsFirstSample = !IsRunning();

    m_samples[m_nextSample] = {aArrivalTime, aCaptureTick};
    m_nextSample = (m_nextSample + 1) % cMaxSamples;
    m_sampleCount = std::min(m_sampleCount + 1, cMaxSamples);

    UpdateEstimates(aArrivalTime);

    if (cIsFirstSample)
    {
        m_lastAdvanceTime = aArrivalTime;
        m_renderTick = GetDesiredRenderTick(aArrivalTime);
    }

    return true;
}

double PlaybackClock::Advance(const double aNow) noexcept
{
    if (!IsRunning())
        return 0.0;

    const double delta = std::max(aNow - m_lastAdvanceTime, 0.0);
    m_lastAdvanceTime = aNow;

    // The playback rate was picked to close the last error. A long frame must not carry the correction past it, or playback
    // would end up off by as much the other way, e.g. ahead of every snapshot that has arrived.
    const double cCorrection = std::clamp(delta * (m_playbackRate - 1.0), std::min(m_error, 0.0), std::max(m_error, 0.0));
    m_renderTick += delta + cCorrection;

    const double desiredRenderTick = GetDesiredRenderTick(aNow);
    m_error = desiredRenderTick - m_renderTick;

    if (std::abs(m_error) > cSnapThreshold)
    {
        m_renderTick = desiredRenderTick;
        m_playbackRate = 1.0;
        m_error = 0.0;
    }
    else
    {
        // Small enough changes in playback speed are not noticeable, unlike jumps in time
        m_playbackRate = 1.0 + std::clamp(m_error / cFullRateAdjustmentError * cMaxRateAdjustment, -cMaxRateAdjustment, cMaxRateAdjustment);
    }

    return delta;
}

void PlaybackClock::UpdateEstimates(const double aNewestArrivalTime) noexcept
{
    std::array<double, cMaxSamples> transits;
    size_t count = 0;
    uint64_t oldestCaptureTick = std::numeric_limits<uint64_t>::max();
    uint64_t newestCaptureTick = 0;

    for (size_t i = 0; i < m_sampleCount; ++i)
    {
        const auto& sample = m_samples[i];
        if (sample.ArrivalTime < aNewestArrivalTime - cSampleWindow)
            continue;

        // Includes the network delay and the unknown offset between both clocks, only differences between transits are meaningful
        transits[count++] = sample.ArrivalTime - static_cast<double>(sample.CaptureTick);
        oldestCaptureTick = std::min(oldestCaptureTick, sample.CaptureTick);
        newestCaptureTick = std::max(newestCaptureTick, sample.CaptureTick);
    }

    // The sample that triggered the update is always within the window
    const auto end = transits.begin() + count;
    m_minTransit = *std::min_element(transits.begin(), end);

    // Rounded up so that few samples give a conservative estimate
    const auto percentile = transits.begin() + static_cast<size_t>(std::ceil(cJitterPercentile * static_cast<double>(count - 1)));
    std::nth_element(transits.begin(), percentile, end);
    m_jitter = *percentile - m_minTransit;

    // An average is enough because owners send at an even pace, see RunLocalUpdates
    if (count >= 2)
        m_snapshotInterval = static_cast<double>(newestCaptureTick - oldestCaptureTick) / static_cast<double>(count - 1);

    m_targetDelay = std::clamp(m_snapshotInterval + m_jitter + cSafetyMargin, cMinTargetDelay, cMaxTargetDelay);
}

double PlaybackClock::GetDesiredRenderTick(const double aNow) const noexcept
{
    // aNow - m_minTransit is the newest owner tick that could have arrived by now
    return aNow - m_minTransit - m_targetDelay;
}
