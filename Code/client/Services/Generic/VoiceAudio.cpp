#include <TiltedOnlinePCH.h>

#include <Services/VoiceAudio.h>
#include <Systems/VoiceDsp.h>

// Only devices are used, leave the decoders, encoders and engine out
#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MA_NO_ENGINE
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_NODE_GRAPH
#define MA_NO_GENERATION
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <opus/opus.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cstring>

namespace
{
// Speech at this bitrate sounds clear, 50 frames a second come to about 3 KB/s per speaker
constexpr int32_t kOpusBitrate = 24000;
constexpr size_t kMaxQueuedFrames = 50;
// Missing frames up to this many are concealed, a longer gap restarts the voice
constexpr int32_t kMaxConcealedFrames = 3;
constexpr auto kActiveSpeakerTime = std::chrono::milliseconds(300);
constexpr char kDefaultDeviceName[] = "default";
} // namespace

struct VoiceAudio::Impl
{
    struct Speaker
    {
        VoiceDsp::JitterBuffer Buffer;
        float LeftGain{0.f};
        float RightGain{0.f};
    };

    // Main thread only
    struct Decoder
    {
        OpusDecoder* pDecoder{nullptr};
        uint16_t LastSequence{0};
        bool HasSequence{false};
        std::chrono::steady_clock::time_point LastFrameTime{};
    };

    ~Impl() noexcept
    {
        StopCapture();
        StopPlayback();

        for (auto& [id, decoder] : Decoders)
            opus_decoder_destroy(decoder.pDecoder);

        if (pEncoder)
            opus_encoder_destroy(pEncoder);

        if (HasContext)
            ma_context_uninit(&Context);
    }

    bool EnsureContext() noexcept
    {
        if (HasContext)
            return true;

        if (ma_context_init(nullptr, 0, nullptr, &Context) != MA_SUCCESS)
        {
            spdlog::error("Voice chat: no audio backend could be started");
            return false;
        }

        HasContext = true;
        return true;
    }

    bool FindDevice(ma_device_type aType, const String& acName, ma_device_id& aOutId) noexcept
    {
        ma_device_info* pPlayback = nullptr;
        ma_uint32 playbackCount = 0;
        ma_device_info* pCapture = nullptr;
        ma_uint32 captureCount = 0;
        if (ma_context_get_devices(&Context, &pPlayback, &playbackCount, &pCapture, &captureCount) != MA_SUCCESS)
            return false;

        ma_device_info* pInfos = aType == ma_device_type_capture ? pCapture : pPlayback;
        const ma_uint32 count = aType == ma_device_type_capture ? captureCount : playbackCount;
        for (ma_uint32 i = 0; i < count; ++i)
        {
            if (acName == pInfos[i].name)
            {
                aOutId = pInfos[i].id;
                return true;
            }
        }

        return false;
    }

    void StartCapture(const String& acName) noexcept
    {
        StopCapture();
        CaptureName = acName;
        if (acName.empty())
            return;

        ma_device_id id{};
        const bool useDefault = acName == kDefaultDeviceName;
        if (!useDefault && !FindDevice(ma_device_type_capture, acName, id))
        {
            spdlog::warn("Voice chat: input device \"{}\" is not connected, nothing is sent", acName);
            return;
        }

        if (!pEncoder)
        {
            int error = OPUS_OK;
            pEncoder = opus_encoder_create(VoiceDsp::kSampleRate, 1, OPUS_APPLICATION_VOIP, &error);
            if (error != OPUS_OK || !pEncoder)
            {
                spdlog::error("Voice chat: Opus encoder failed to start ({})", error);
                pEncoder = nullptr;
                return;
            }

            opus_encoder_ctl(pEncoder, OPUS_SET_BITRATE(kOpusBitrate));
            opus_encoder_ctl(pEncoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));
        }

        ma_device_config config = ma_device_config_init(ma_device_type_capture);
        config.capture.format = ma_format_f32;
        config.capture.channels = 1;
        config.capture.pDeviceID = useDefault ? nullptr : &id;
        config.sampleRate = VoiceDsp::kSampleRate;
        config.periodSizeInMilliseconds = 10;
        config.dataCallback = &Impl::OnCapture;
        config.pUserData = this;

        CaptureFill = 0;
        if (ma_device_init(&Context, &config, &CaptureDevice) != MA_SUCCESS)
        {
            spdlog::error("Voice chat: input device \"{}\" could not be opened", acName);
            return;
        }

        HasCapture = true;
        if (ma_device_start(&CaptureDevice) != MA_SUCCESS)
        {
            spdlog::error("Voice chat: input device \"{}\" could not be started", acName);
            StopCapture();
            return;
        }

        spdlog::info("Voice chat: capturing from \"{}\"", CaptureDevice.capture.name);
    }

    void StopCapture() noexcept
    {
        if (HasCapture)
        {
            ma_device_uninit(&CaptureDevice);
            HasCapture = false;
        }

        MicLevel = 0.f;
    }

    void StartPlayback(const String& acName) noexcept
    {
        StopPlayback();
        PlaybackName = acName;

        ma_device_id id{};
        bool useDefault = acName.empty() || acName == kDefaultDeviceName;
        if (!useDefault && !FindDevice(ma_device_type_playback, acName, id))
        {
            spdlog::warn("Voice chat: output device \"{}\" is not connected, using the Windows default", acName);
            useDefault = true;
        }

        ma_device_config config = ma_device_config_init(ma_device_type_playback);
        config.playback.format = ma_format_f32;
        config.playback.channels = 2;
        config.playback.pDeviceID = useDefault ? nullptr : &id;
        config.sampleRate = VoiceDsp::kSampleRate;
        config.periodSizeInMilliseconds = 10;
        config.dataCallback = &Impl::OnPlayback;
        config.pUserData = this;

        if (ma_device_init(&Context, &config, &PlaybackDevice) != MA_SUCCESS)
        {
            spdlog::error("Voice chat: output device could not be opened");
            return;
        }

        HasPlayback = true;
        if (ma_device_start(&PlaybackDevice) != MA_SUCCESS)
        {
            spdlog::error("Voice chat: output device could not be started");
            StopPlayback();
            return;
        }

        spdlog::info("Voice chat: playing on \"{}\"", PlaybackDevice.playback.name);
    }

    void StopPlayback() noexcept
    {
        if (HasPlayback)
        {
            ma_device_uninit(&PlaybackDevice);
            HasPlayback = false;
        }
    }

    // Capture thread
    static void OnCapture(ma_device* apDevice, void*, const void* apInput, ma_uint32 aFrameCount)
    {
        auto* pSelf = static_cast<Impl*>(apDevice->pUserData);
        const auto* pInput = static_cast<const float*>(apInput);
        if (!pSelf || !pInput)
            return;

        for (ma_uint32 i = 0; i < aFrameCount; ++i)
        {
            pSelf->CaptureFrame[pSelf->CaptureFill++] = pInput[i];
            if (pSelf->CaptureFill == VoiceDsp::kFrameSamples)
            {
                pSelf->CaptureFill = 0;
                pSelf->ProcessCapturedFrame();
            }
        }
    }

    // Capture thread
    void ProcessCapturedFrame() noexcept
    {
        const float volume = MicVolume.load(std::memory_order_relaxed);
        for (float& sample : CaptureFrame)
            sample = std::clamp(sample * volume, -1.f, 1.f);

        const float peak = VoiceDsp::Peak(CaptureFrame.data(), CaptureFrame.size());
        MicLevel.store(peak, std::memory_order_relaxed);

        const bool open = Gate.Process(peak, NoiseGate.load(std::memory_order_relaxed));
        if (!open || !Transmitting.load(std::memory_order_relaxed) || !pEncoder)
            return;

        uint8_t packet[VoiceFrame::kMaxSize];
        const opus_int32 size = opus_encode_float(pEncoder, CaptureFrame.data(), VoiceDsp::kFrameSamples, packet, sizeof(packet));
        if (size <= 0)
            return;

        VoiceFrame frame{};
        frame.Sequence = NextSequence++;
        frame.Data.assign(packet, packet + size);

        std::scoped_lock lock(OutgoingMutex);
        if (Outgoing.size() >= kMaxQueuedFrames)
            Outgoing.erase(Outgoing.begin());
        Outgoing.push_back(std::move(frame));
    }

    // Playback thread
    static void OnPlayback(ma_device* apDevice, void* apOutput, const void*, ma_uint32 aFrameCount)
    {
        auto* pSelf = static_cast<Impl*>(apDevice->pUserData);
        auto* pOutput = static_cast<float*>(apOutput);
        if (!pOutput)
            return;

        std::memset(pOutput, 0, sizeof(float) * 2 * aFrameCount);
        if (!pSelf)
            return;

        {
            std::scoped_lock lock(pSelf->SpeakersMutex);
            // Map values are only writable through the iterator
            for (auto itor = pSelf->Speakers.begin(); itor != pSelf->Speakers.end(); ++itor)
            {
                auto& speaker = itor.value();
                speaker.Buffer.MixInto(pOutput, aFrameCount, speaker.LeftGain, speaker.RightGain);
            }
        }

        for (ma_uint32 i = 0; i < aFrameCount * 2; ++i)
            pOutput[i] = std::clamp(pOutput[i], -1.f, 1.f);
    }

    ma_context Context{};
    bool HasContext{false};

    ma_device CaptureDevice{};
    bool HasCapture{false};
    String CaptureName{};
    ma_device PlaybackDevice{};
    bool HasPlayback{false};
    String PlaybackName{};
    bool Running{false};

    // Capture thread state
    OpusEncoder* pEncoder{nullptr};
    std::array<float, VoiceDsp::kFrameSamples> CaptureFrame{};
    uint32_t CaptureFill{0};
    VoiceDsp::NoiseGate Gate{};
    uint16_t NextSequence{0};

    std::atomic<float> MicVolume{1.f};
    std::atomic<float> NoiseGate{0.1f};
    std::atomic<float> MicLevel{0.f};
    std::atomic<bool> Transmitting{false};

    std::mutex OutgoingMutex;
    Vector<VoiceFrame> Outgoing;

    std::mutex SpeakersMutex;
    Map<uint32_t, Speaker> Speakers;

    Map<uint32_t, Decoder> Decoders;
};

VoiceAudio::VoiceAudio() noexcept
    : m_pImpl(TiltedPhoques::MakeUnique<Impl>())
{
}

VoiceAudio::~VoiceAudio() noexcept = default;

bool VoiceAudio::ListDevices(DeviceLists& aOutLists) noexcept
{
    aOutLists.Inputs.clear();
    aOutLists.Outputs.clear();

    if (!m_pImpl->EnsureContext())
        return false;

    ma_device_info* pPlayback = nullptr;
    ma_uint32 playbackCount = 0;
    ma_device_info* pCapture = nullptr;
    ma_uint32 captureCount = 0;
    if (ma_context_get_devices(&m_pImpl->Context, &pPlayback, &playbackCount, &pCapture, &captureCount) != MA_SUCCESS)
        return false;

    for (ma_uint32 i = 0; i < captureCount; ++i)
        aOutLists.Inputs.push_back(pCapture[i].name);
    for (ma_uint32 i = 0; i < playbackCount; ++i)
        aOutLists.Outputs.push_back(pPlayback[i].name);

    return true;
}

void VoiceAudio::Start(const String& acInputDevice, const String& acOutputDevice) noexcept
{
    if (!m_pImpl->EnsureContext())
        return;

    if (!m_pImpl->Running || acInputDevice != m_pImpl->CaptureName)
        m_pImpl->StartCapture(acInputDevice);
    if (!m_pImpl->Running || acOutputDevice != m_pImpl->PlaybackName)
        m_pImpl->StartPlayback(acOutputDevice);

    m_pImpl->Running = true;
}

void VoiceAudio::Stop() noexcept
{
    m_pImpl->StopCapture();
    m_pImpl->StopPlayback();
    m_pImpl->Running = false;

    {
        std::scoped_lock lock(m_pImpl->OutgoingMutex);
        m_pImpl->Outgoing.clear();
    }

    RemoveAllSpeakers();
}

bool VoiceAudio::IsRunning() const noexcept
{
    return m_pImpl->Running;
}

void VoiceAudio::SetCaptureSettings(float aMicVolume, float aNoiseGate) noexcept
{
    m_pImpl->MicVolume.store(std::clamp(aMicVolume, 0.f, 1.f), std::memory_order_relaxed);
    m_pImpl->NoiseGate.store(std::clamp(aNoiseGate, 0.f, 1.f), std::memory_order_relaxed);
}

void VoiceAudio::SetTransmitting(bool aTransmitting) noexcept
{
    m_pImpl->Transmitting.store(aTransmitting, std::memory_order_relaxed);
}

float VoiceAudio::GetMicLevel() const noexcept
{
    return m_pImpl->HasCapture ? m_pImpl->MicLevel.load(std::memory_order_relaxed) : 0.f;
}

bool VoiceAudio::PopEncodedFrame(VoiceFrame& aOutFrame) noexcept
{
    std::scoped_lock lock(m_pImpl->OutgoingMutex);
    if (m_pImpl->Outgoing.empty())
        return false;

    aOutFrame = std::move(m_pImpl->Outgoing.front());
    m_pImpl->Outgoing.erase(m_pImpl->Outgoing.begin());
    return true;
}

void VoiceAudio::PushRemoteFrame(uint32_t aPlayerId, const VoiceFrame& acFrame) noexcept
{
    if (!m_pImpl->Running || acFrame.Data.empty())
        return;

    auto& decoder = m_pImpl->Decoders[aPlayerId];
    if (!decoder.pDecoder)
    {
        int error = OPUS_OK;
        decoder.pDecoder = opus_decoder_create(VoiceDsp::kSampleRate, 1, &error);
        if (error != OPUS_OK || !decoder.pDecoder)
        {
            spdlog::error("Voice chat: Opus decoder failed to start ({})", error);
            m_pImpl->Decoders.erase(aPlayerId);
            return;
        }
    }

    // Late and duplicate frames are dropped, a few missing ones are concealed by Opus
    int32_t concealed = 0;
    if (decoder.HasSequence)
    {
        const int32_t delta = VoiceDsp::SequenceDelta(decoder.LastSequence, acFrame.Sequence);
        if (delta <= 0)
            return;

        concealed = std::min(delta - 1, kMaxConcealedFrames);
    }

    decoder.LastSequence = acFrame.Sequence;
    decoder.HasSequence = true;
    decoder.LastFrameTime = std::chrono::steady_clock::now();

    std::array<float, VoiceDsp::kFrameSamples * (kMaxConcealedFrames + 1)> pcm{};
    size_t samples = 0;

    for (int32_t i = 0; i < concealed; ++i)
    {
        const int decoded = opus_decode_float(decoder.pDecoder, nullptr, 0, pcm.data() + samples, VoiceDsp::kFrameSamples, 0);
        if (decoded > 0)
            samples += static_cast<size_t>(decoded);
    }

    const int decoded = opus_decode_float(decoder.pDecoder, acFrame.Data.data(), static_cast<opus_int32>(acFrame.Data.size()), pcm.data() + samples, VoiceDsp::kFrameSamples, 0);
    if (decoded > 0)
        samples += static_cast<size_t>(decoded);

    if (samples == 0)
        return;

    std::scoped_lock lock(m_pImpl->SpeakersMutex);
    m_pImpl->Speakers[aPlayerId].Buffer.Push(pcm.data(), samples);
}

void VoiceAudio::SetSpeakerGains(uint32_t aPlayerId, float aLeft, float aRight) noexcept
{
    std::scoped_lock lock(m_pImpl->SpeakersMutex);
    const auto itor = m_pImpl->Speakers.find(aPlayerId);
    if (itor == m_pImpl->Speakers.end())
        return;

    itor.value().LeftGain = aLeft;
    itor.value().RightGain = aRight;
}

void VoiceAudio::RemoveSpeaker(uint32_t aPlayerId) noexcept
{
    {
        std::scoped_lock lock(m_pImpl->SpeakersMutex);
        m_pImpl->Speakers.erase(aPlayerId);
    }

    const auto itor = m_pImpl->Decoders.find(aPlayerId);
    if (itor != m_pImpl->Decoders.end())
    {
        opus_decoder_destroy(itor->second.pDecoder);
        m_pImpl->Decoders.erase(itor);
    }
}

void VoiceAudio::RemoveAllSpeakers() noexcept
{
    {
        std::scoped_lock lock(m_pImpl->SpeakersMutex);
        m_pImpl->Speakers.clear();
    }

    for (auto& [id, decoder] : m_pImpl->Decoders)
        opus_decoder_destroy(decoder.pDecoder);
    m_pImpl->Decoders.clear();
}

void VoiceAudio::GetActiveSpeakers(Vector<uint32_t>& aOutPlayerIds) const noexcept
{
    aOutPlayerIds.clear();

    const auto now = std::chrono::steady_clock::now();
    for (const auto& [id, decoder] : m_pImpl->Decoders)
    {
        if (now - decoder.LastFrameTime < kActiveSpeakerTime)
            aOutPlayerIds.push_back(id);
    }
}
