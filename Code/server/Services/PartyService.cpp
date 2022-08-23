#include <Services/PartyService.h>
#include <Components.h>
#include <GameServer.h>

#include <Events/PlayerJoinEvent.h>
#include <Events/PlayerLeaveEvent.h>
#include <Events/UpdateEvent.h>

#include <Messages/NotifyPlayerList.h>
#include <Messages/NotifyPartyInfo.h>
#include <Messages/NotifyPartyInvite.h>
#include <Messages/PartyInviteRequest.h>
#include <Messages/PartyAcceptInviteRequest.h>
#include <Messages/PartyLeaveRequest.h>
#include <Messages/NotifyPartyJoined.h>
#include <Messages/NotifyPartyLeft.h>
#include <Messages/PartyCreateRequest.h>
#include <Messages/PartyChangeLeaderRequest.h>
#include <Messages/PartyKickRequest.h>
#include <Messages/NotifyPlayerJoined.h>

PartyService::PartyService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
    , m_updateEvent(aDispatcher.sink<UpdateEvent>().connect<&PartyService::OnUpdate>(this))
    , m_playerJoinConnection(aDispatcher.sink<PlayerJoinEvent>().connect<&PartyService::OnPlayerJoin>(this))
    , m_playerLeaveConnection(aDispatcher.sink<PlayerLeaveEvent>().connect<&PartyService::OnPlayerLeave>(this))
    , m_partyInviteConnection(aDispatcher.sink<PacketEvent<PartyInviteRequest>>().connect<&PartyService::OnPartyInvite>(this))
    , m_partyAcceptInviteConnection(aDispatcher.sink<PacketEvent<PartyAcceptInviteRequest>>().connect<&PartyService::OnPartyAcceptInvite>(this))
    , m_partyLeaveConnection(aDispatcher.sink<PacketEvent<PartyLeaveRequest>>().connect<&PartyService::OnPartyLeave>(this))
    , m_partyCreateConnection(aDispatcher.sink<PacketEvent<PartyCreateRequest>>().connect<&PartyService::OnPartyCreate>(this))
    , m_partyChangeLeaderConnection(aDispatcher.sink<PacketEvent<PartyChangeLeaderRequest>>().connect<&PartyService::OnPartyChangeLeader>(this)),
      m_partyKickConnection(aDispatcher.sink<PacketEvent<PartyKickRequest>>().connect<&PartyService::OnPartyKick>(this))
{
}

const PartyService::Party* PartyService::GetById(uint32_t aId) const noexcept
{
    auto itor = m_parties.find(aId);
    if (itor != std::end(m_parties))
        return &itor->second;

    return nullptr;
}

bool PartyService::IsPlayerInParty(Player* const apPlayer) const noexcept
{
    return apPlayer->GetParty().JoinedPartyId.has_value();
}

bool PartyService::IsPlayerLeader(Player* const apPlayer) noexcept
{
    auto& inviterPartyComponent = apPlayer->GetParty();
    if (inviterPartyComponent.JoinedPartyId)
    {
        Party& party = m_parties[*inviterPartyComponent.JoinedPartyId];
        return party.LeaderPlayerId == apPlayer->GetId();
    }

    return false;
}

PartyService::Party* PartyService::GetPlayerParty(Player* const apPlayer) noexcept
{
    auto& inviterPartyComponent = apPlayer->GetParty();
    if (inviterPartyComponent.JoinedPartyId)
    {
        return &m_parties[*inviterPartyComponent.JoinedPartyId];
    }

    return nullptr;
}

void PartyService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    const auto cCurrentTick = GameServer::Get()->GetTick();
    if (m_nextInvitationExpire > cCurrentTick)
        return;

    // Only expire once every 10 seconds
    m_nextInvitationExpire = cCurrentTick + 10000;

    auto view = m_world.view<PartyComponent>();
    for (auto entity : view)
    {
        auto& partyComponent = view.get<PartyComponent>(entity);
        auto itor = std::begin(partyComponent.Invitations);
        while (itor != std::end(partyComponent.Invitations))
        {
            if (itor->second < cCurrentTick)
            {
                itor = partyComponent.Invitations.erase(itor);
            }
            else
            {
                ++itor;
            }
        }
    }
}

void PartyService::OnPartyCreate(const PacketEvent<PartyCreateRequest>& acPacket) noexcept
{
    Player *const player = acPacket.pPlayer;
    auto& inviterPartyComponent = player->GetParty();

    spdlog::debug("[PartyService]: Received request to create party");

    if (!inviterPartyComponent.JoinedPartyId) // Ensure not in party
    {
        uint32_t partyId = m_nextId++;
        Party& party = m_parties[partyId];
        party.Members.push_back(player);
        party.LeaderPlayerId = player->GetId();
        inviterPartyComponent.JoinedPartyId = partyId;

        spdlog::debug("[PartyService]: Created party for {}", player->GetId());
        SendPartyJoinedEvent(party, player);
    }
}

void PartyService::OnPartyChangeLeader(const PacketEvent<PartyChangeLeaderRequest>& acPacket) noexcept
{
    auto& message = acPacket.Packet;
    Player* const player = acPacket.pPlayer;
    Player* const pNewLeader = m_world.GetPlayerManager().GetById(message.PartyMemberPlayerId);

    spdlog::debug("[PartyService]: Received request to change party leader to {}", message.PartyMemberPlayerId);

    if (!pNewLeader) 
    {
        spdlog::error("[PartyService]: Player {} does not exist¨. Cannot change paty leader", message.PartyMemberPlayerId);
        return;
    }

    auto& inviterPartyComponent = player->GetParty();
    if (inviterPartyComponent.JoinedPartyId) // Ensure not in party
    {
        Party& party = m_parties[*inviterPartyComponent.JoinedPartyId];
        if (party.LeaderPlayerId == player->GetId())
        {
            for (auto& pPlayer : party.Members)
            {
                if (pPlayer->GetId() == pNewLeader->GetId())
                {
                    party.LeaderPlayerId = pPlayer->GetId();
                    spdlog::debug("[PartyService]: Changed party leader to {}, updating party members.", party.LeaderPlayerId);
                    BroadcastPartyInfo(*inviterPartyComponent.JoinedPartyId);
                    break;
                }
            }
        }
    }
}

void PartyService::OnPartyKick(const PacketEvent<PartyKickRequest>& acPacket) noexcept
{
    auto& message = acPacket.Packet;
    Player* const player = acPacket.pPlayer;
    Player* const pKick = m_world.GetPlayerManager().GetById(message.PartyMemberPlayerId);

    spdlog::debug("[PartyService]: Received request to change party leader to {}", message.PartyMemberPlayerId);

    if (!pKick) 
    {
        spdlog::error("[PartyService]: Player {} does not exist. Cannot kick", message.PartyMemberPlayerId);
        return;
    }

    auto& inviterPartyComponent = player->GetParty();
    if (inviterPartyComponent.JoinedPartyId) // Ensure not in party
    {
        Party& party = m_parties[*inviterPartyComponent.JoinedPartyId];
        if (party.LeaderPlayerId == player->GetId())
        {
            spdlog::debug("[PartyService]: Kicking player {} from party", pKick->GetId());
            RemovePlayerFromParty(pKick);
            BroadcastPlayerList(pKick);
        }
    }
}

void PartyService::OnPlayerJoin(const PlayerJoinEvent& acEvent) const noexcept
{
    BroadcastPlayerList();

    NotifyPlayerJoined notify{};
    notify.PlayerId = acEvent.pPlayer->GetId();
    notify.Username = acEvent.pPlayer->GetUsername();

    notify.WorldSpaceId = acEvent.WorldSpaceId;
    notify.CellId = acEvent.CellId;

    notify.Level = acEvent.pPlayer->GetLevel();

    spdlog::debug("[Party] New notify player {:x} {}", notify.PlayerId, notify.Username.c_str());

    GameServer::Get()->SendToPlayers(notify, acEvent.pPlayer);
}

void PartyService::OnPartyInvite(const PacketEvent<PartyInviteRequest>& acPacket) noexcept
{
    auto& message = acPacket.Packet;

    // Make sure the player we invite exists
    Player* const pInvitee = m_world.GetPlayerManager().GetById(message.PlayerId);
    Player* const pInviter = acPacket.pPlayer;

    // If both players are available and they are different
    if (pInvitee && pInvitee != pInviter)
    {
        auto& inviterPartyComponent = pInviter->GetParty();
        auto& inviteePartyComponent = pInvitee->GetParty();

        spdlog::debug("[PartyService]: Got party invite from {}", pInviter->GetId());

        if (!inviterPartyComponent.JoinedPartyId)
        {
            spdlog::debug("[PartyService]: Inviter not in party, cancelling invite.");
            return;
        }
        else if (inviteePartyComponent.JoinedPartyId)
        {
            spdlog::debug("[PartyService]: Invitee in party already, cancelling invite.");
            return;
        }

        auto& party = m_parties[*inviterPartyComponent.JoinedPartyId];
        if (party.LeaderPlayerId != pInviter->GetId())
        {
            spdlog::debug("[PartyService]: Inviter not party leader, cancelling invite.");
            return;
        }

        // Expire in 60 seconds
        const auto cExpiryTick = GameServer::Get()->GetTick() + 60000;
        inviteePartyComponent.Invitations[pInviter] = cExpiryTick;

        NotifyPartyInvite notification;
        notification.InviterId = pInviter->GetId();
        notification.ExpiryTick = cExpiryTick;

        spdlog::debug("[PartyService]: Sending party invite to {}", pInvitee->GetId());
        pInvitee->Send(notification);
    }
}

void PartyService::OnPartyAcceptInvite(const PacketEvent<PartyAcceptInviteRequest>& acPacket) noexcept
{
    auto& message = acPacket.Packet;

    Player* const pInviter = m_world.GetPlayerManager().GetById(message.InviterId);
    Player* pSelf = acPacket.pPlayer;

    spdlog::debug("[PartyService]: Got party accept request from {}", pSelf->GetId());

    // If both players are available and they are different
    if (pInviter && pInviter != pSelf)
    {
        auto& inviterPartyComponent = pInviter->GetParty();
        auto& selfPartyComponent = pSelf->GetParty();

        // Check if we have this invitation so people don't invite themselves
        if (selfPartyComponent.Invitations.count(pInviter) == 0)
            return;

        spdlog::debug("[PartyService]: Invite found, processing.");
        if (!inviterPartyComponent.JoinedPartyId) // Ensure inviter is in a party otherwise break
        {
            spdlog::debug("[PartyService]: Inviter not in party. Cancelling.");
            return;
        }

        auto partyId = *inviterPartyComponent.JoinedPartyId;
        Party& party = m_parties[partyId];

        if (party.LeaderPlayerId != pInviter->GetId())
        {
            spdlog::debug("[PartyService]: Inviter is not party leader. Cancelling.");
            return;
        }

        if (selfPartyComponent.JoinedPartyId) // Remove from party if in one already. TODO: Decide if player needs to be out of party first
        {
            spdlog::debug("[PartyService]: Invitee already in party, cancelling.");
            // RemovePlayerFromParty(pSelf, false); // skip sending left event, will override with SendPartyJoinedEvent
            return;
        }

        party.Members.push_back(pSelf);
        selfPartyComponent.JoinedPartyId = partyId;

        spdlog::debug("[PartyService]: Added invitee to party, sending events");
        SendPartyJoinedEvent(party, pSelf);
        BroadcastPartyInfo(partyId);
    }
}

void PartyService::OnPartyLeave(const PacketEvent<PartyLeaveRequest>& acPacket) noexcept
{
    RemovePlayerFromParty(acPacket.pPlayer);
}

void PartyService::OnPlayerLeave(const PlayerLeaveEvent& acEvent) noexcept
{
    RemovePlayerFromParty(acEvent.pPlayer);
    BroadcastPlayerList(acEvent.pPlayer);
}

void PartyService::RemovePlayerFromParty(Player* apPlayer) noexcept
{
    auto* pPartyComponent = &apPlayer->GetParty();
    spdlog::debug("[PartyService]: Removing player from party.");

    if (pPartyComponent->JoinedPartyId)
    {
        auto id = *pPartyComponent->JoinedPartyId;

        Party& party = m_parties[id];
        auto& members = party.Members;

        members.erase(std::find(std::begin(members), std::end(members), apPlayer));

        if (members.empty())
        {
            m_parties.erase(id);
        }
        else
        {
            if (party.LeaderPlayerId == apPlayer->GetId())
            {
                party.LeaderPlayerId = members.at(0)->GetId(); // Reassign party leader
                spdlog::debug("[PartyService]: Leader left, reassigned party leader to {}", party.LeaderPlayerId);
            }
            spdlog::debug("[PartyService]: Updating other party players of removal.");
            BroadcastPartyInfo(id);
        }

        pPartyComponent->JoinedPartyId.reset();

        spdlog::debug("[PartyService]: Sending party left event to player.");
        NotifyPartyLeft leftMessage;
        apPlayer->Send(leftMessage);
    }
}

void PartyService::BroadcastPlayerList(Player* apPlayer) const noexcept
{
    auto pIgnoredPlayer = apPlayer;
    for (auto pSelf : m_world.GetPlayerManager())
    {
        if (pIgnoredPlayer == pSelf)
            continue;

        NotifyPlayerList playerList;
        for (auto pPlayer : m_world.GetPlayerManager())
        {
            if (pSelf == pPlayer)
                continue;

            if (pIgnoredPlayer == pPlayer)
                continue;

            playerList.Players[pPlayer->GetId()] = pPlayer->GetUsername();
        }

        pSelf->Send(playerList);
    }
}

void PartyService::BroadcastPartyInfo(uint32_t aPartyId) const noexcept
{
    auto itor = m_parties.find(aPartyId);
    if (itor == std::end(m_parties))
        return;

    auto& party = itor->second;
    auto& members = party.Members;

    NotifyPartyInfo message;
    message.LeaderPlayerId = party.LeaderPlayerId;

    for (auto pPlayer : members)
    {
        message.PlayerIds.push_back(pPlayer->GetId());
    }

    for (auto pPlayer : members)
    {
        message.IsLeader = pPlayer->GetId() == party.LeaderPlayerId;
        pPlayer->Send(message);
    }
}

void PartyService::SendPartyJoinedEvent(Party& aParty, Player* aPlayer) noexcept
{
    NotifyPartyJoined joinedMessage;
    joinedMessage.LeaderPlayerId = aParty.LeaderPlayerId;
    joinedMessage.IsLeader = aParty.LeaderPlayerId == aPlayer->GetId();
    for (auto pPlayer : aParty.Members)
    {
        joinedMessage.PlayerIds.push_back(pPlayer->GetId());
    }
    spdlog::debug("[PartyService]: Sending party join event to player");
    aPlayer->Send(joinedMessage);
}
