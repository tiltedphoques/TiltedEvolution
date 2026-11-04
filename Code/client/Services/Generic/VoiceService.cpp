#include <TiltedOnlinePCH.h>

#include <Services/VoiceService.h>
#include <Systems/VoiceDsp.h>

#include <World.h>
#include <Components.h>

#include <Events/ConnectedEvent.h>
#include <Events/DisconnectedEvent.h>
#include <Events/UpdateEvent.h>

#include <Messages/NotifyPlayerLeft.h>
#include <Messages/NotifyVoiceData.h>
#include <Messages/VoiceDataRequest.h>
#include <Messages/VoiceStateRequest.h>

#include <Actor.h>
#include <PlayerCharacter.h>
#include <Camera/PlayerCamera.h>
#include <NetImmerse/NiNode.h>

#include <OverlayApp.hpp>

#include <base/simpleini/SimpleIni.h>

#include <cmath>
#include <cstddef>

namespace
{
// Same file the dedicated server uses when it runs from the game folder, so bVoip is one switch for both
constexpr char kSettingsPath[] = "Data/SkyrimTogetherReborn/config/STServer.ini";
constexpr char kSection[] = "Voice";

constexpr float kMaxFadeStart = 100000.f;
// How often talking icons and the mic meter are pushed to the UI
constexpr double kUiInterval = 0.066;
// Voices are panned this much at most, fully one sided sounds odd on headphones
constexpr float kMaxPan = 0.7f;

// NiAVObject::world (NiTransform), checked in Ghidra on 1.7.104: NiAVObject::UpdateWorldData (0x140eebf40) writes
// world = parent world * local to 0x7C..0xAF, row major rotate at 0x7C, translate at 0xA0. The camera position getter
// (ID 50785) reads cameraRoot (TESCamera +0x20) world translate at 0xA0, and the engine's audio listener update
// (ID 40449) uses a camera node's world rotate Y column as forward.
constexpr size_t kNiAVObjectWorldRotateOffset = 0x7C;
constexpr size_t kNiAVObjectWorldTranslateOffset = 0xA0;
static_assert(kNiAVObjectWorldTranslateOffset + sizeof(float) * 3 <= sizeof(NiAVObject));
// TESCamera's camera root node pointer. Read by offset because the member is named cameraNode (NiNode*) on our
// master and cameraRoot (NiPointer<NiNode>) on upstream dev, both a single pointer at 0x20.
constexpr size_t kTESCameraRootOffset = 0x20;
static_assert(kTESCameraRootOffset + sizeof(NiNode*) <= sizeof(TESCamera));

struct Listener
{
    NiPoint3 Position{};
    // Horizontal, unit length, east/north
    float RightX{1.f};
    float RightY{0.f};
};

// Voices are heard from the camera, so free look, camera mods and ragdolls don't matter, unless the camera is far away
bool GetListener(Listener& aOutListener, float aMaxCameraDistance) noexcept
{
    PlayerCharacter* pPlayer = PlayerCharacter::Get();
    if (!pPlayer)
        return false;

    aOutListener.Position = pPlayer->position;
    // Skyrim heading is clockwise from north, so facing is (sin, cos) and right is (cos, -sin)
    aOutListener.RightX = std::cos(pPlayer->rotation.z);
    aOutListener.RightY = -std::sin(pPlayer->rotation.z);

    PlayerCamera* pCamera = PlayerCamera::Get();
    // PlayerCamera redeclares the TESCamera members at the wrong offsets, go through the base on purpose
    const NiNode* pCameraRoot = nullptr;
    if (pCamera)
        std::memcpy(&pCameraRoot, reinterpret_cast<const uint8_t*>(static_cast<TESCamera*>(pCamera)) + kTESCameraRootOffset, sizeof(pCameraRoot));
    if (!pCameraRoot)
        return true;

    const auto* pBytes = reinterpret_cast<const uint8_t*>(pCameraRoot);
    float rotate[3][3];
    float translate[3];
    std::memcpy(rotate, pBytes + kNiAVObjectWorldRotateOffset, sizeof(rotate));
    std::memcpy(translate, pBytes + kNiAVObjectWorldTranslateOffset, sizeof(translate));

    if (!std::isfinite(translate[0]) || !std::isfinite(translate[1]) || !std::isfinite(translate[2]))
        return true;

    // The world map, free camera and some cutscenes move the camera far from the character. Past the fade start, hear
    // from the character instead, so nearby players don't go quiet while you look at the map. Using the player's own
    // fade start ties both to one setting. Vanilla third person zoomed all the way out is about 750 units.
    const float dx = translate[0] - pPlayer->position.x;
    const float dy = translate[1] - pPlayer->position.y;
    const float dz = translate[2] - pPlayer->position.z;
    if (dx * dx + dy * dy + dz * dz > aMaxCameraDistance * aMaxCameraDistance)
        return true;

    aOutListener.Position = glm::vec3(translate[0], translate[1], translate[2]);

    const float frontX = rotate[0][1];
    const float frontY = rotate[1][1];
    const float length = std::sqrt(frontX * frontX + frontY * frontY);
    // Looking straight up or down, keep the character's facing
    if (std::isfinite(length) && length > 0.01f)
    {
        aOutListener.RightX = frontY / length;
        aOutListener.RightY = -frontX / length;
    }

    return true;
}

const char* ChannelToString(VoiceService::Channel aChannel) noexcept
{
    switch (aChannel)
    {
    case VoiceService::Channel::kParty: return "party";
    case VoiceService::Channel::kStrangers: return "strangers";
    default: return "everyone";
    }
}

VoiceService::Channel ChannelFromString(const std::string_view acValue) noexcept
{
    if (acValue == "party")
        return VoiceService::Channel::kParty;
    if (acValue == "strangers")
        return VoiceService::Channel::kStrangers;
    return VoiceService::Channel::kEveryone;
}

float Clamp01(double aValue, float aFallback) noexcept
{
    return std::isfinite(aValue) ? std::clamp(static_cast<float>(aValue), 0.f, 1.f) : aFallback;
}

std::filesystem::path GetSettingsPath() noexcept
{
    std::error_code error{};
    return std::filesystem::current_path(error) / kSettingsPath;
}
} // namespace

VoiceService::VoiceService(World& aWorld, entt::dispatcher& aDispatcher, TransportService& aTransport) noexcept
    : m_world(aWorld)
    , m_transport(aTransport)
{
    LoadSettings();
    m_audio.SetCaptureSettings(m_settings.MicVolume, m_settings.NoiseGate);

    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&VoiceService::OnUpdate>(this);
    m_connectedConnection = aDispatcher.sink<ConnectedEvent>().connect<&VoiceService::OnConnected>(this);
    m_disconnectedConnection = aDispatcher.sink<DisconnectedEvent>().connect<&VoiceService::OnDisconnected>(this);
    m_voiceDataConnection = aDispatcher.sink<NotifyVoiceData>().connect<&VoiceService::OnVoiceData>(this);
    m_playerLeftConnection = aDispatcher.sink<NotifyPlayerLeft>().connect<&VoiceService::OnPlayerLeft>(this);
}

VoiceService::~VoiceService() noexcept
{
    m_audio.Stop();
}

void VoiceService::RefreshUi() noexcept
{
    // The ini may have been edited by hand or by the server since the menu was last open
    LoadSettings();
    m_audio.SetCaptureSettings(m_settings.MicVolume, m_settings.NoiseGate);

    auto* pOverlay = m_world.GetOverlayService().GetOverlayApp();
    if (pOverlay)
    {
        VoiceAudio::DeviceLists devices{};
        m_audio.ListDevices(devices);

        auto pInputs = CefListValue::Create();
        for (size_t i = 0; i < devices.Inputs.size(); ++i)
            pInputs->SetString(i, devices.Inputs[i].c_str());

        auto pOutputs = CefListValue::Create();
        for (size_t i = 0; i < devices.Outputs.size(); ++i)
            pOutputs->SetString(i, devices.Outputs[i].c_str());

        auto pArguments = CefListValue::Create();
        pArguments->SetList(0, pInputs);
        pArguments->SetList(1, pOutputs);
        pOverlay->ExecuteAsync("voiceDevices", pArguments);
    }

    SendSettingsToUi();
    UpdateAudioState();
}

void VoiceService::ApplySettings(const Settings& acSettings) noexcept
{
    m_settings = acSettings;
    m_settings.FadeStart = std::isfinite(m_settings.FadeStart) ? std::clamp(m_settings.FadeStart, 0.f, kMaxFadeStart) : 2000.f;
    m_settings.MicVolume = Clamp01(m_settings.MicVolume, 1.f);
    m_settings.NoiseGate = Clamp01(m_settings.NoiseGate, 0.05f);
    m_settings.MasterVolume = VoiceDsp::ClampMasterVolume(m_settings.MasterVolume);

    m_audio.SetCaptureSettings(m_settings.MicVolume, m_settings.NoiseGate);
    SaveSettings();
    UpdateAudioState();
}

void VoiceService::SetPlayerVolume(uint32_t aPlayerId, float aVolume) noexcept
{
    m_playerVolumes[aPlayerId] = Clamp01(aVolume, 1.f);
}

void VoiceService::SetMeterEnabled(bool aEnabled) noexcept
{
    m_meterEnabled = aEnabled;
    UpdateAudioState();
}

void VoiceService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    UpdateAudioState();

    if (m_audio.IsRunning())
    {
        SendOutgoingFrames();
        UpdateSpeakerGains();
    }

    UpdateUi(acEvent.Delta);
}

void VoiceService::OnConnected(const ConnectedEvent&) noexcept
{
    // A new server knows nothing about us yet
    m_listeningSent = false;
}

void VoiceService::OnDisconnected(const DisconnectedEvent&) noexcept
{
    m_listeningSent = false;
    m_playerVolumes.clear();
    UpdateAudioState();
}

void VoiceService::OnVoiceData(const NotifyVoiceData& acMessage) noexcept
{
    if (!IsVoiceWanted())
        return;

    m_audio.PushRemoteFrame(acMessage.PlayerId, acMessage.Frame);
}

void VoiceService::OnPlayerLeft(const NotifyPlayerLeft& acMessage) noexcept
{
    m_audio.RemoveSpeaker(acMessage.PlayerId);
    m_playerVolumes.erase(acMessage.PlayerId);
}

bool VoiceService::IsVoiceWanted() const noexcept
{
    return m_settings.Connect && m_transport.IsOnline() && m_world.GetServerSettings().VoipEnabled;
}

void VoiceService::UpdateAudioState() noexcept
{
    const bool wanted = IsVoiceWanted();
    // The mic popup's meter works with voip off too, it only reads the microphone locally
    const bool metering = m_meterEnabled && !m_settings.InputDevice.empty();

    if (wanted || metering)
        m_audio.Start(m_settings.InputDevice, m_settings.OutputDevice);
    else if (m_audio.IsRunning())
        m_audio.Stop();

    m_audio.SetTransmitting(wanted && !m_settings.InputDevice.empty());

    if (!wanted && m_audio.IsRunning())
        m_audio.RemoveAllSpeakers();

    SendListeningState();
}

void VoiceService::SendListeningState() noexcept
{
    if (!m_transport.IsOnline())
        return;

    const bool listening = IsVoiceWanted();
    if (m_listeningSent && m_sentListening == listening && m_sentChannel == m_settings.ListenChannel)
        return;

    VoiceStateRequest request{};
    request.Listening = listening;
    request.ListenChannel = static_cast<VoiceStateRequest::Channel>(m_settings.ListenChannel);
    if (!m_transport.Send(request))
        return;

    m_listeningSent = true;
    m_sentListening = listening;
    m_sentChannel = m_settings.ListenChannel;
}

void VoiceService::SendOutgoingFrames() noexcept
{
    VoiceDataRequest request{};
    while (m_audio.PopEncodedFrame(request.Frame))
    {
        if (!IsVoiceWanted())
            continue;

        // Late voice is useless, so no resends, and no Nagle delay either
        m_transport.Send(request, TiltedPhoques::kUnreliableNoNagle);
    }
}

void VoiceService::UpdateSpeakerGains() noexcept
{
    Vector<uint32_t> speakers{};
    m_audio.GetActiveSpeakers(speakers);
    if (speakers.empty())
        return;

    Listener listener{};
    const bool hasListener = GetListener(listener, m_settings.FadeStart);

    const auto& partyMembers = m_world.GetPartyService().GetPartyMembers();
    auto view = m_world.view<FormIdComponent, PlayerComponent>();

    for (const uint32_t playerId : speakers)
    {
        float left = 0.f;
        float right = 0.f;

        const bool inParty = std::find(partyMembers.begin(), partyMembers.end(), playerId) != partyMembers.end();
        const bool channelAllows = m_settings.ListenChannel == Channel::kEveryone || (m_settings.ListenChannel == Channel::kParty && inParty) ||
                                   (m_settings.ListenChannel == Channel::kStrangers && !inParty);

        const auto volumeItor = m_playerVolumes.find(playerId);
        const float volume = volumeItor != m_playerVolumes.end() ? volumeItor->second : 1.f;

        // A player whose character isn't loaded here is too far away to be heard
        const auto entityItor = std::find_if(view.begin(), view.end(), [&view, playerId](auto aEntity) { return view.get<PlayerComponent>(aEntity).Id == playerId; });
        Actor* pActor = entityItor != view.end() ? Cast<Actor>(TESForm::GetById(view.get<FormIdComponent>(*entityItor).Id)) : nullptr;

        if (hasListener && channelAllows && volume > 0.f && pActor)
        {
            const float dx = pActor->position.x - listener.Position.x;
            const float dy = pActor->position.y - listener.Position.y;
            const float dz = pActor->position.z - listener.Position.z;
            const float distance = std::sqrt(dx * dx + dy * dy + dz * dz);

            const float gain = m_settings.MasterVolume * volume * VoiceDsp::DistanceGain(distance, m_settings.FadeStart);

            const float horizontal = std::sqrt(dx * dx + dy * dy);
            const float pan = horizontal > 1.f ? (dx * listener.RightX + dy * listener.RightY) / horizontal * kMaxPan : 0.f;

            VoiceDsp::PanGains(pan, left, right);
            left *= gain;
            right *= gain;
        }

        m_audio.SetSpeakerGains(playerId, left, right);
    }
}

void VoiceService::UpdateUi(double aDelta) noexcept
{
    m_uiTimer += aDelta;
    if (m_uiTimer < kUiInterval)
        return;
    m_uiTimer = 0.0;

    auto* pOverlay = m_world.GetOverlayService().GetOverlayApp();
    if (!pOverlay)
        return;

    // Players whose voice is getting through their noise gate right now
    Vector<uint32_t> talking{};
    if (IsVoiceWanted())
        m_audio.GetActiveSpeakers(talking);
    std::sort(talking.begin(), talking.end());

    if (talking != m_lastTalking)
    {
        m_lastTalking = talking;

        auto pIds = CefListValue::Create();
        for (size_t i = 0; i < talking.size(); ++i)
            pIds->SetInt(i, static_cast<int>(talking[i]));

        auto pArguments = CefListValue::Create();
        pArguments->SetList(0, pIds);
        pOverlay->ExecuteAsync("voiceTalking", pArguments);
    }

    if (m_meterEnabled)
    {
        auto pArguments = CefListValue::Create();
        pArguments->SetInt(0, static_cast<int>(std::lround(m_audio.GetMicLevel() * 100.f)));
        pOverlay->ExecuteAsync("voiceMicLevel", pArguments);
    }
}

void VoiceService::LoadSettings() noexcept
{
    Settings settings{};

    const auto path = GetSettingsPath();
    std::error_code error{};
    if (std::filesystem::exists(path, error) && !error)
    {
        CSimpleIni ini(true);
        if (ini.LoadFile(path.c_str()) == SI_Error::SI_OK)
        {
            settings.InputDevice = ini.GetValue(kSection, "sInputDevice", "");
            settings.Connect = ini.GetBoolValue(kSection, "bVoip", false);
            settings.OutputDevice = ini.GetValue(kSection, "sOutputDevice", "");
            settings.ListenChannel = ChannelFromString(ini.GetValue(kSection, "sVoipChannel", "everyone"));
            settings.FadeStart = static_cast<float>(ini.GetDoubleValue(kSection, "fVoipFadeStart", 2000.0));
            settings.MicVolume = static_cast<float>(ini.GetDoubleValue(kSection, "fMicVolume", 1.0));
            settings.NoiseGate = static_cast<float>(ini.GetDoubleValue(kSection, "fNoiseGate", 0.05));
            settings.MasterVolume = static_cast<float>(ini.GetDoubleValue(kSection, "fVoipMasterVolume", 1.0));
        }
        else
        {
            spdlog::warn("Voice chat: {} could not be read, using the defaults", kSettingsPath);
        }
    }

    settings.FadeStart = std::isfinite(settings.FadeStart) ? std::clamp(settings.FadeStart, 0.f, kMaxFadeStart) : 2000.f;
    settings.MicVolume = Clamp01(settings.MicVolume, 1.f);
    settings.NoiseGate = Clamp01(settings.NoiseGate, 0.05f);
    settings.MasterVolume = VoiceDsp::ClampMasterVolume(settings.MasterVolume);

    m_settings = settings;
}

void VoiceService::SaveSettings() const noexcept
{
    const auto path = GetSettingsPath();

    std::error_code error{};
    std::filesystem::create_directories(path.parent_path(), error);

    // Load first so the server's own settings in the file are kept
    CSimpleIni ini(true);
    if (std::filesystem::exists(path, error) && !error && ini.LoadFile(path.c_str()) != SI_Error::SI_OK)
    {
        spdlog::error("Voice chat: {} could not be read, voice settings are not saved", kSettingsPath);
        return;
    }

    ini.SetValue(kSection, "sInputDevice", m_settings.InputDevice.c_str(), "; Player: microphone for voice chat. Empty sends nothing, default follows the Windows default device");
    ini.SetBoolValue(kSection, "bVoip", m_settings.Connect,
                     "; Built-in proximity voice chat. On a server: lets players talk to players nearby. For a player: the Connect to voip checkbox in Settings");
    ini.SetValue(kSection, "sOutputDevice", m_settings.OutputDevice.c_str(), "; Player: where voices play. Empty uses the Windows default device, like the game");
    ini.SetValue(kSection, "sVoipChannel", ChannelToString(m_settings.ListenChannel), "; Player: who you hear, everyone, party or strangers");
    ini.SetDoubleValue(kSection, "fVoipFadeStart", m_settings.FadeStart, "; Player: voices are full volume up to this distance in game units, silent at 4 times it");
    ini.SetDoubleValue(kSection, "fVoipMasterVolume", m_settings.MasterVolume, "; Player: all received voices, 0 to 5 (0-500%), default 1 (100%)");
    ini.SetDoubleValue(kSection, "fMicVolume", m_settings.MicVolume, "; Player: microphone volume, 0 to 1");
    ini.SetDoubleValue(kSection, "fNoiseGate", m_settings.NoiseGate, "; Player: microphone level below which nothing is sent, 0 to 1 like the Windows mic test bar");

    if (ini.SaveFile(path.c_str(), false) != SI_Error::SI_OK)
        spdlog::error("Voice chat: could not write {}", kSettingsPath);
}

void VoiceService::SendSettingsToUi() const noexcept
{
    auto* pOverlay = m_world.GetOverlayService().GetOverlayApp();
    if (!pOverlay)
        return;

    auto pArguments = CefListValue::Create();
    pArguments->SetString(0, m_settings.InputDevice.c_str());
    pArguments->SetInt(1, m_settings.Connect ? 1 : 0);
    pArguments->SetString(2, m_settings.OutputDevice.c_str());
    pArguments->SetString(3, ChannelToString(m_settings.ListenChannel));
    pArguments->SetInt(4, static_cast<int>(std::lround(m_settings.FadeStart)));
    pArguments->SetInt(5, static_cast<int>(std::lround(m_settings.MicVolume * 100.f)));
    pArguments->SetInt(6, static_cast<int>(std::lround(m_settings.NoiseGate * 100.f)));
    pArguments->SetInt(7, static_cast<int>(std::lround(m_settings.MasterVolume * 100.f)));
    pOverlay->ExecuteAsync("voiceSettings", pArguments);
}
