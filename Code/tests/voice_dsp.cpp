#include <limits>
#include <catch2/catch.hpp>

#include "../client/Systems/VoiceDsp.h"

#include <vector>

using namespace VoiceDsp;

TEST_CASE("Voice distance falloff", "[voice.dsp]")
{
    REQUIRE(DistanceGain(0.f, 2000.f) == 1.f);
    REQUIRE(DistanceGain(2000.f, 2000.f) == 1.f);
    // -6 dB per doubling: half the amplitude at twice the fade start, a third at three times before the taper
    REQUIRE(DistanceGain(3000.f, 2000.f) == Approx(2.f / 3.f));
    REQUIRE(DistanceGain(4000.f, 2000.f) == Approx(0.5f));
    // Halfway through the taper the inverse law (1/3) is halved
    REQUIRE(DistanceGain(6000.f, 2000.f) == Approx(1.f / 6.f));
    REQUIRE(DistanceGain(8000.f, 2000.f) == 0.f);
    REQUIRE(DistanceGain(9000.f, 2000.f) == 0.f);

    // Never gets louder further away
    float previous = 1.f;
    for (float distance = 0.f; distance <= 8400.f; distance += 50.f)
    {
        const float gain = DistanceGain(distance, 2000.f);
        REQUIRE(gain <= previous);
        previous = gain;
    }

    // A fade start of 0 would divide by zero, it means nobody is heard
    REQUIRE(DistanceGain(10.f, 0.f) == 0.f);
}

TEST_CASE("Voice panning keeps loudness", "[voice.dsp]")
{
    float left = 0.f, right = 0.f;

    PanGains(0.f, left, right);
    REQUIRE(left == Approx(right));
    REQUIRE(left * left + right * right == Approx(1.f));

    PanGains(-1.f, left, right);
    REQUIRE(left == Approx(1.f));
    REQUIRE(right == Approx(0.f).margin(1e-6));

    PanGains(5.f, left, right);
    REQUIRE(right == Approx(1.f));
}

TEST_CASE("Voice sequence numbers wrap", "[voice.dsp]")
{
    REQUIRE(SequenceDelta(10, 11) == 1);
    REQUIRE(SequenceDelta(10, 13) == 3);
    REQUIRE(SequenceDelta(65535, 0) == 1);
    REQUIRE(SequenceDelta(65534, 2) == 4);
    REQUIRE(SequenceDelta(11, 10) == -1);
    REQUIRE(SequenceDelta(10, 10) == 0);
}

TEST_CASE("Voice noise gate", "[voice.dsp]")
{
    NoiseGate gate;

    // Below the threshold stays closed
    REQUIRE_FALSE(gate.Process(0.05f, 0.1f));

    // Opens on a loud frame and holds through a pause
    REQUIRE(gate.Process(0.3f, 0.1f));
    for (uint32_t i = 1; i < NoiseGate::kHoldFrames; ++i)
        REQUIRE(gate.Process(0.01f, 0.1f));
    REQUIRE_FALSE(gate.Process(0.01f, 0.1f));

    // A threshold of 0 sends anything but digital silence
    NoiseGate openGate;
    REQUIRE(openGate.Process(0.001f, 0.f));

    REQUIRE(Peak(std::vector<float>{0.1f, -0.4f, 0.2f}.data(), 3) == Approx(0.4f));
}

TEST_CASE("Voice jitter buffer", "[voice.dsp]")
{
    JitterBuffer buffer;
    std::vector<float> frame(kFrameSamples, 0.5f);
    std::vector<float> out(kFrameSamples * 2, 0.f);

    // Waits for 60 ms before playing
    buffer.Push(frame.data(), frame.size());
    buffer.Push(frame.data(), frame.size());
    REQUIRE_FALSE(buffer.IsPlaying());
    REQUIRE(buffer.MixInto(out.data(), kFrameSamples, 1.f, 1.f) == 0);

    buffer.Push(frame.data(), frame.size());
    REQUIRE(buffer.IsPlaying());

    REQUIRE(buffer.MixInto(out.data(), kFrameSamples, 1.f, 0.5f) == kFrameSamples);
    REQUIRE(out[0] == Approx(0.5f));
    REQUIRE(out[1] == Approx(0.25f));

    // Runs dry and stops until it has buffered again
    buffer.MixInto(out.data(), kFrameSamples, 1.f, 1.f);
    buffer.MixInto(out.data(), kFrameSamples, 1.f, 1.f);
    REQUIRE_FALSE(buffer.IsPlaying());
    REQUIRE(buffer.GetBuffered() == 0);

    // Falls too far behind and drops the oldest audio
    for (int i = 0; i < 12; ++i)
        buffer.Push(frame.data(), frame.size());
    REQUIRE(buffer.GetBuffered() <= JitterBuffer::kMaxSamples);
    REQUIRE(buffer.IsPlaying());
}

TEST_CASE("Voice master volume supports amplification and rejects invalid settings", "[voice.dsp]")
{
    REQUIRE(ClampMasterVolume(0.f) == 0.f);
    REQUIRE(ClampMasterVolume(1.f) == 1.f);
    REQUIRE(ClampMasterVolume(2.5f) == 2.5f);
    REQUIRE(ClampMasterVolume(5.f) == 5.f);
    REQUIRE(ClampMasterVolume(10.f) == 5.f);
    REQUIRE(ClampMasterVolume(-1.f) == 0.f);
    REQUIRE(ClampMasterVolume(std::numeric_limits<float>::quiet_NaN()) == 1.f);
    REQUIRE(ClampMasterVolume(std::numeric_limits<float>::infinity()) == 1.f);

    JitterBuffer buffer;
    std::vector<float> frame(JitterBuffer::kStartSamples, 0.1f);
    std::vector<float> out(6, 0.f);
    buffer.Push(frame.data(), frame.size());
    REQUIRE(buffer.MixInto(out.data(), 1, 2.5f, 2.5f) == 1);
    REQUIRE(out[0] == Approx(0.25f));
    REQUIRE(buffer.MixInto(out.data() + 2, 1, 5.f, 5.f) == 1);
    REQUIRE(out[2] == Approx(0.5f));
    REQUIRE(buffer.MixInto(out.data() + 4, 1, 0.f, 0.f) == 1);
    REQUIRE(out[4] == 0.f);
}
