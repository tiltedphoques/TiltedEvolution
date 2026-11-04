#include <Services/VoiceService.h>

#include <Components.h>
#include <GameServer.h>
#include <World.h>

#include <Events/PlayerLeaveEvent.h>

#include <Messages/NotifyVoiceData.h>
#include <Messages/VoiceDataRequest.h>
#include <Messages/VoiceStateRequest.h>

#include <console/Setting.h>

extern Console::Setting<bool> bVoip;

namespace
{
// A speaker sends 50 frames a second, this leaves room for bursts after a stall without letting a client flood
constexpr uint32_t kMaxFramesPerSecond = 75;
} // namespace

VoiceService::VoiceService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
{
    m_voiceDataConnection = aDispatcher.sink<PacketEvent<VoiceDataRequest>>().connect<&VoiceService::OnVoiceDataRequest>(this);
    m_voiceStateConnection = aDispatcher.sink<PacketEvent<VoiceStateRequest>>().connect<&VoiceService::OnVoiceStateRequest>(this);
    m_playerLeaveConnection = aDispatcher.sink<PlayerLeaveEvent>().connect<&VoiceService::OnPlayerLeave>(this);
}

void VoiceService::OnVoiceDataRequest(const PacketEvent<VoiceDataRequest>& acMessage) noexcept
{
    if (!bVoip)
        return;

    const auto& frame = acMessage.Packet.Frame;
    if (frame.Data.empty())
        return;

    Player* const pSpeaker = acMessage.pPlayer;
    if (!IsWithinRateLimit(pSpeaker->GetId()))
        return;

    // Only players who are in the world can be heard
    const auto speakerCharacter = pSpeaker->GetCharacter();
    if (!speakerCharacter)
        return;

    const auto* pSpeakerCell = m_world.try_get<CellIdComponent>(*speakerCharacter);
    const CellIdComponent& speakerCell = pSpeakerCell ? *pSpeakerCell : pSpeaker->GetCellComponent();
    const auto& speakerParty = pSpeaker->GetParty().JoinedPartyId;

    NotifyVoiceData notify{};
    notify.PlayerId = pSpeaker->GetId();
    notify.Frame = frame;

    for (Player* pListener : m_world.GetPlayerManager())
    {
        if (pListener == pSpeaker)
            continue;

        const auto listenerItor = m_listeners.find(pListener->GetId());
        if (listenerItor == m_listeners.end() || !listenerItor->second.Listening)
            continue;

        const auto& listenerParty = pListener->GetParty().JoinedPartyId;
        const bool sameParty = speakerParty.has_value() && listenerParty.has_value() && *speakerParty == *listenerParty;

        const auto channel = listenerItor->second.Channel;
        if (channel == VoiceStateRequest::kParty && !sameParty)
            continue;
        if (channel == VoiceStateRequest::kStrangers && sameParty)
            continue;

        if (!speakerCell.IsInRange(pListener->GetCellComponent(), false))
            continue;

        // Late voice is useless, so no resends, and no Nagle delay either
        pListener->Send(notify, TiltedPhoques::kUnreliableNoNagle);
    }
}

void VoiceService::OnVoiceStateRequest(const PacketEvent<VoiceStateRequest>& acMessage) noexcept
{
    auto& listener = m_listeners[acMessage.pPlayer->GetId()];
    listener.Listening = acMessage.Packet.Listening;
    listener.Channel = acMessage.Packet.ListenChannel;
}

void VoiceService::OnPlayerLeave(const PlayerLeaveEvent& acEvent) noexcept
{
    m_listeners.erase(acEvent.pPlayer->GetId());
    m_speakers.erase(acEvent.pPlayer->GetId());
}

bool VoiceService::IsWithinRateLimit(uint32_t aPlayerId) noexcept
{
    const auto now = std::chrono::steady_clock::now();
    auto& speaker = m_speakers[aPlayerId];

    if (now - speaker.WindowStart >= std::chrono::seconds(1))
    {
        speaker.WindowStart = now;
        speaker.FramesInWindow = 0;
    }

    return ++speaker.FramesInWindow <= kMaxFramesPerSecond;
}
