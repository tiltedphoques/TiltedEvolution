#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

/**
 * @brief Picks which owner tick of a remote actor to show, just far enough behind for the next snapshot to be there.
 *
 * The delay comes from capture ticks vs. arrival times, so the clocks needn't be in sync, and changes are eased in by
 * playing slightly faster or slower. Times are ms on one monotonic clock. Arrivals should be network receive times, so
 * our own long frames don't look like jitter.
 */
struct PlaybackClock
{
    // Returns false for a duplicate
    bool AddSample(double aArrivalTime, uint64_t aCaptureTick) noexcept;
    // Returns the time elapsed since the last call
    double Advance(double aNow) noexcept;

    [[nodiscard]] bool IsRunning() const noexcept { return m_sampleCount > 0; }
    // The owner tick to show now
    [[nodiscard]] double GetRenderTick() const noexcept { return m_renderTick; }
    [[nodiscard]] double GetTargetDelay() const noexcept { return m_targetDelay; }
    [[nodiscard]] double GetJitter() const noexcept { return m_jitter; }
    [[nodiscard]] double GetSnapshotInterval() const noexcept { return m_snapshotInterval; }
    [[nodiscard]] double GetPlaybackRate() const noexcept { return m_playbackRate; }

    // Estimates use the samples that arrived within this window
    static constexpr double cSampleWindow = 3000.0;
    static constexpr size_t cMaxSamples = 128;
    // Until two snapshots have arrived
    static constexpr double cDefaultSnapshotInterval = 100.0;
    // Later arrivals are left to extrapolation
    static constexpr double cJitterPercentile = 0.95;
    static constexpr double cSafetyMargin = 8.0;
    static constexpr double cMinTargetDelay = 16.0;
    static constexpr double cMaxTargetDelay = 250.0;
    // Most playback may speed up or slow down while catching up
    static constexpr double cMaxRateAdjustment = 0.1;
    // Error in ms that gets the full rate adjustment
    static constexpr double cFullRateAdjustmentError = 50.0;
    // Larger errors jump instead of catching up
    static constexpr double cSnapThreshold = 250.0;

private:
    struct Sample
    {
        double ArrivalTime;
        uint64_t CaptureTick;
    };

    void UpdateEstimates(double aNewestArrivalTime) noexcept;
    [[nodiscard]] double GetDesiredRenderTick(double aNow) const noexcept;

    std::array<Sample, cMaxSamples> m_samples{};
    size_t m_sampleCount{0};
    size_t m_nextSample{0};

    double m_minTransit{0.0};
    double m_jitter{0.0};
    double m_snapshotInterval{cDefaultSnapshotInterval};
    double m_targetDelay{cDefaultSnapshotInterval + cSafetyMargin};

    double m_renderTick{0.0};
    double m_lastAdvanceTime{0.0};
    double m_playbackRate{1.0};
    // Left from the last Advance(), the playback rate is set to close it
    double m_error{0.0};
};
