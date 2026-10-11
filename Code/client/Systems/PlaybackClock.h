#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

/**
 * @brief Picks which owner tick of a remote actor to show, just far enough behind for the next snapshot to be there.
 *
 * What it looks like in game:
 * - Remote actors are shown a little in the past, their ping plus about one snapshot interval. E.g. ~65 ms for a player
 *   at 20 ms ping, more for NPCs out of combat, which are sent less often.
 * - Too little delay and playback runs out of snapshots: the actor keeps moving on its own for a moment and is pulled back
 *   when the late snapshot arrives, see InterpolationSystem. Too much delay and the actor lags, e.g. attacks show up late.
 * - When the delay has to change, the actor moves slightly faster or slower for a moment instead of jumping. It only jumps
 *   when playback is far off, e.g. after the owner's game froze.
 * The "Component debug" window shows these numbers live for the selected actor, the "Network" window can simulate a bad
 * connection.
 *
 * How the delay is picked:
 *   renderTick = now - minTransit - targetDelay
 * - Transit is a snapshot's arrival time minus its capture tick: the network delay plus the unknown offset between both
 *   clocks, so the clocks needn't be in sync. now - minTransit is the newest owner tick that could have arrived by now.
 * - targetDelay = snapshot interval + jitter + safety margin, long enough for the next snapshot to arrive in most cases.
 *
 * Ticks are on the owner's clock, times on ours, both in ms. Arrival times should be network receive times, so our own
 * long frames don't look like jitter.
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

    // Estimates only use snapshots that arrived within this window, so they follow changing network conditions
    static constexpr double cSampleWindow = 3000.0;
    // Holds a whole window of snapshots at the ~30 per second players are sent at
    static constexpr size_t cMaxSamples = 128;
    // Assumed until two snapshots have arrived, as slow as NPCs out of combat are sent
    static constexpr double cDefaultSnapshotInterval = 100.0;
    // How many of the newest gaps between snapshots are checked for the owner sending less often
    static constexpr size_t cRecentGaps = 4;
    // Playback waits for all but the latest 5% of snapshots, the actor briefly extrapolates over those instead
    static constexpr double cJitterPercentile = 0.95;
    // Extra delay for small variations the estimates miss, like uneven frame times
    static constexpr double cSafetyMargin = 8.0;
    // A floor for safety, the snapshot interval alone is already longer
    static constexpr double cMinTargetDelay = 16.0;
    // On a very bad connection the actor extrapolates more rather than lagging further and further behind
    static constexpr double cMaxTargetDelay = 250.0;
    // Playback runs at most 10% faster or slower while catching up, too little to notice
    static constexpr double cMaxRateAdjustment = 0.1;
    // How far off in ms playback has to be for the full speed change, smaller errors get proportionally less
    static constexpr double cFullRateAdjustmentError = 50.0;
    // Further off than this, playback jumps instead, catching up at 10% would take seconds. Unrelated to cMaxTargetDelay.
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
