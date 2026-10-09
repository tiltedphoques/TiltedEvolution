#include <catch2/catch.hpp>

#include "../client/Systems/PlaybackClock.h"

#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

namespace
{
constexpr double cFrameTime = 1000.0 / 60.0;
// Estimates settle within the sample window, only judge the clock after that
constexpr double cWarmup = PlaybackClock::cSampleWindow;

struct NetworkConditions
{
    // Like RunLocalUpdates, the owner sends on the first of its frames that is at least this long after its previous snapshot
    double SnapshotInterval = 30.0;
    double OwnerFrameTime = 1000.0 / 60.0;
    // Each owner frame is up to this much shorter or longer
    double OwnerFrameJitter = 0.0;
    double Latency = 10.0;
    // Every snapshot is delayed by a uniformly random extra amount up to this
    double MaxJitter = 0.0;
    // The owner's clock reads this much more than the local one, the clock must not care
    uint64_t OwnerClockOffset = 1'000'000;
    // Our frame starting at HitchTime takes this much longer, snapshots keep arriving meanwhile
    double HitchTime = 0.0;
    double HitchLength = 0.0;
    double Duration = 10000.0;
    uint32_t Seed = 1;
};

struct SimulationResult
{
    // How far behind the owner the displayed tick is, network latency included
    double MaxDelay = 0.0;
    double MeanDelay = 0.0;
    // Frames that displayed a tick no snapshot had reached yet, i.e. that had to extrapolate
    size_t StarvedFrames = 0;
    size_t Frames = 0;
    // Whether playback always advanced at close to real time speed, never jumped or ran backwards
    bool IsAlwaysSmooth = true;
};

// Snapshots are captured on the owner's frames, arrive after the network delay and are handled on the receiver's next frame
SimulationResult Simulate(const NetworkConditions& acConditions)
{
    struct InFlightSnapshot
    {
        double ArrivalTime;
        uint64_t CaptureTick;
    };

    std::mt19937 random(acConditions.Seed);
    std::uniform_real_distribution<double> jitter(0.0, acConditions.MaxJitter);
    std::uniform_real_distribution<double> frameJitter(-acConditions.OwnerFrameJitter, acConditions.OwnerFrameJitter);

    std::vector<InFlightSnapshot> snapshots;
    double lastCaptureTime = -acConditions.SnapshotInterval;
    for (double captureTime = 0.0; captureTime < acConditions.Duration;
         captureTime += acConditions.OwnerFrameTime + (acConditions.OwnerFrameJitter > 0.0 ? frameJitter(random) : 0.0))
    {
        if (captureTime - lastCaptureTime < acConditions.SnapshotInterval)
            continue;

        lastCaptureTime = captureTime;

        const double cArrivalTime = captureTime + acConditions.Latency + (acConditions.MaxJitter > 0.0 ? jitter(random) : 0.0);
        snapshots.push_back({cArrivalTime, acConditions.OwnerClockOffset + static_cast<uint64_t>(captureTime)});
    }

    std::sort(snapshots.begin(), snapshots.end(), [](const auto& acLhs, const auto& acRhs) { return acLhs.ArrivalTime < acRhs.ArrivalTime; });

    PlaybackClock clock;
    SimulationResult result;
    size_t nextSnapshot = 0;
    uint64_t newestCaptureTick = 0;
    double totalDelay = 0.0;

    for (double now = 0.0; now < acConditions.Duration; now += cFrameTime)
    {
        if (now >= acConditions.HitchTime && now < acConditions.HitchTime + cFrameTime)
            now += acConditions.HitchLength;

        while (nextSnapshot < snapshots.size() && snapshots[nextSnapshot].ArrivalTime <= now)
        {
            // Stamped with when the network received it, not when this frame got to it
            clock.AddSample(snapshots[nextSnapshot].ArrivalTime, snapshots[nextSnapshot].CaptureTick);
            newestCaptureTick = std::max(newestCaptureTick, snapshots[nextSnapshot].CaptureTick);
            ++nextSnapshot;
        }

        const double cPreviousRenderTick = clock.GetRenderTick();
        const double cDeltaTime = clock.Advance(now);

        if (!clock.IsRunning() || now < cWarmup)
            continue;

        const double cRenderTick = clock.GetRenderTick();
        const double cAdvanced = cRenderTick - cPreviousRenderTick;
        const double cMaxDeviation = cDeltaTime * PlaybackClock::cMaxRateAdjustment + 1e-6;
        if (std::abs(cAdvanced - cDeltaTime) > cMaxDeviation)
            result.IsAlwaysSmooth = false;

        const double cDelay = static_cast<double>(acConditions.OwnerClockOffset) + now - cRenderTick;
        result.MaxDelay = std::max(result.MaxDelay, cDelay);
        totalDelay += cDelay;

        if (cRenderTick > static_cast<double>(newestCaptureTick))
            ++result.StarvedFrames;

        ++result.Frames;
    }

    result.MeanDelay = result.Frames ? totalDelay / static_cast<double>(result.Frames) : 0.0;
    return result;
}
} // namespace

TEST_CASE("Playback clock", "[client.playback_clock]")
{
    SECTION("Stays well under the old fixed 300ms delay on a stable connection")
    {
        const auto result = Simulate({});

        REQUIRE(result.Frames > 0);
        REQUIRE(result.StarvedFrames == 0);
        REQUIRE(result.IsAlwaysSmooth);
        // 10ms latency + the gap between snapshots + safety margin
        REQUIRE(result.MeanDelay > 10.0);
        REQUIRE(result.MaxDelay < 90.0);
    }

    SECTION("Buffers more on a jittery connection and rarely runs out of snapshots")
    {
        NetworkConditions conditions;
        conditions.MaxJitter = 40.0;
        conditions.Duration = 30000.0;

        const auto result = Simulate(conditions);

        REQUIRE(result.IsAlwaysSmooth);
        REQUIRE(result.MeanDelay > Simulate({}).MeanDelay);
        REQUIRE(result.MaxDelay < 160.0);
        REQUIRE(static_cast<double>(result.StarvedFrames) < 0.05 * static_cast<double>(result.Frames));
    }

    SECTION("Plays low rate actors further behind instead of starving them")
    {
        NetworkConditions conditions;
        conditions.SnapshotInterval = 100.0;

        const auto result = Simulate(conditions);

        REQUIRE(result.StarvedFrames == 0);
        REQUIRE(result.IsAlwaysSmooth);
        REQUIRE(result.MaxDelay < 160.0);
    }

    SECTION("Keeps up with a 60 fps owner whose frame times vary")
    {
        NetworkConditions conditions;
        conditions.OwnerFrameJitter = 2.0;
        conditions.Duration = 30000.0;

        const auto result = Simulate(conditions);

        REQUIRE(result.IsAlwaysSmooth);
        REQUIRE(static_cast<double>(result.StarvedFrames) < 0.01 * static_cast<double>(result.Frames));
        REQUIRE(result.MaxDelay < 90.0);
    }

    SECTION("Is not thrown off by a long frame on our side")
    {
        NetworkConditions conditions;
        conditions.HitchTime = 5000.0;
        conditions.HitchLength = 800.0;

        const auto result = Simulate(conditions);

        REQUIRE(result.IsAlwaysSmooth);
        REQUIRE(result.StarvedFrames == 0);
        // About the same as without the hitch
        REQUIRE(result.MaxDelay < Simulate({}).MaxDelay + 10.0);
    }

    SECTION("A long frame does not carry a catch-up past where playback should be")
    {
        // Playback starts with a conservative delay and speeds up a little once the second snapshot shows it can do with less
        PlaybackClock clock;
        double now = 0.0;
        uint64_t tick = 1000;
        clock.AddSample(now, tick);
        clock.Advance(now);

        now += 33.0;
        tick += 33;
        clock.AddSample(now, tick);
        clock.Advance(now);

        REQUIRE(clock.GetPlaybackRate() > 1.0);

        // Snapshots keep arriving during one long frame of ours
        while (now < 1000.0)
        {
            now += 33.0;
            tick += 33;
            clock.AddSample(now, tick);
        }
        clock.Advance(now);

        // Caught up exactly, rather than overshooting and having to slow down again
        REQUIRE(clock.GetPlaybackRate() == Approx(1.0).margin(0.01));
    }

    SECTION("Does not depend on the owner's clock matching ours")
    {
        NetworkConditions nearClock;
        NetworkConditions farClock;
        farClock.OwnerClockOffset = 1'712'345'678'901;

        REQUIRE(Simulate(nearClock).MeanDelay == Approx(Simulate(farClock).MeanDelay).margin(0.01));
    }

    SECTION("Jumps instead of crawling when the owner's timeline jumps")
    {
        PlaybackClock clock;
        double now = 0.0;
        uint64_t tick = 1000;
        for (; now < 1000.0; tick += 33, now += 33.0)
        {
            clock.AddSample(now, tick);
            clock.Advance(now);
        }

        // E.g. a new owner whose clock is far ahead of the previous one
        tick += 10000;
        clock.AddSample(now, tick);

        const double cRenderTick = clock.GetRenderTick();
        clock.Advance(now);

        REQUIRE(clock.GetPlaybackRate() == 1.0);
        REQUIRE(clock.GetRenderTick() - cRenderTick > 9000.0);
        REQUIRE(clock.GetRenderTick() < static_cast<double>(tick));
    }

    SECTION("Ignores duplicate snapshots")
    {
        PlaybackClock clock;
        REQUIRE(clock.AddSample(0.0, 1000));
        REQUIRE(clock.AddSample(33.0, 1033));
        REQUIRE_FALSE(clock.AddSample(40.0, 1033));

        REQUIRE(clock.GetSnapshotInterval() == Approx(33.0));
        REQUIRE(clock.GetJitter() == Approx(0.0));
    }
}
