#include <stdafx.h>

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

PartyService::PartyService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
    , m_updateEvent(aDispatcher.sink<UpdateEvent>().connect<&PartyService::OnUpdate>(this))
    , m_playerJoinConnection(aDispatcher.sink<PlayerJoinEvent>().connect<&PartyService::OnPlayerJoin>(this))
    , m_playerLeaveConnection(aDispatcher.sink<PlayerLeaveEvent>().connect<&PartyService::OnPlayerLeave>(this))
    , m_partyInviteConnection(aDispatcher.sink<PacketEvent<PartyInviteRequest>>().connect<&PartyService::OnPartyInvite>(this))
    , m_partyAcceptInviteConnection(aDispatcher.sink<PacketEvent<PartyAcceptInviteRequest>>().connect<&PartyService::OnPartyAcceptInvite>(this))
    , m_partyLeaveConnection(aDispatcher.sink<PacketEvent<PartyLeaveRequest>>().connect<&PartyService::OnPartyLeave>(this))
{
}

const PartyService::Party* PartyService::GetById(uint32_t aId) const noexcept
{
    auto itor = m_parties.find(aId);
    if (itor != std::end(m_parties))
        return &itor->second;

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

void PartyService::OnPlayerJoin(const PlayerJoinEvent& acEvent) const noexcept
{
    BroadcastPlayerList();
}

void PartyService::OnPartyInvite(const PacketEvent<PartyInviteRequest>& acPacket) const noexcept
{
    auto& message = acPacket.Packet;

    // Make sure the player we invite exists
    const auto pOtherPlayer = m_world.GetPlayerManager().GetById(message.PlayerId);
    const auto pSelf = acPacket.pPlayer;

    // If both players are available and they are different
    if (pOtherPlayer && pOtherPlayer != pSelf)
    {
        auto& otherPartyComponent = pOtherPlayer->GetParty();

        // Expire in 60 seconds
        const auto cExpiryTick = GameServer::Get()->GetTick() + 60000;
        otherPartyComponent.Invitations[pSelf] = cExpiryTick;

        NotifyPartyInvite notification;
        notification.InviterId = pSelf->GetId();
        notification.ExpiryTick = cExpiryTick;

        pOtherPlayer->Send(notification);
    }
}

void PartyService::OnPartyAcceptInvite(const PacketEvent<PartyAcceptInviteRequest>& acPacket) noexcept
{
    auto& message = acPacket.Packet;

    const auto pOtherPlayer = m_world.GetPlayerManager().GetById(message.InviterId);
    auto pSelf = acPacket.pPlayer;

    // If both players are available and they are different
    if (pOtherPlayer && pOtherPlayer != pSelf)
    {
        auto& inviterPartyComponent = pOtherPlayer->GetParty();
        auto& selfPartyComponent = pSelf->GetParty();

        auto partyId = 0;

        // Check if we have this invitation so people don't invite themselves
        if (selfPartyComponent.Invitations.count(pOtherPlayer) == 0)
            return;

        // If the inviter isn't in a party, create it
        if (!inviterPartyComponent.JoinedPartyId)
        {
            partyId = m_nextId++;

            auto& party = m_parties[partyId];
            party.Members.push_back(pOtherPlayer);
            inviterPartyComponent.JoinedPartyId = partyId;
        }
        else
        {
            partyId = *inviterPartyComponent.JoinedPartyId;
        }

        auto& party = m_parties[partyId];
        party.Members.push_back(pSelf);
        selfPartyComponent.JoinedPartyId = partyId;

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

    if (pPartyComponent->JoinedPartyId)
    {
        auto id = *pPartyComponent->JoinedPartyId;

        auto& party = m_parties[id];
        auto& members = party.Members;

        members.erase(std::find(std::begin(members), std::end(members), apPlayer));

        if (members.empty())
        {
            m_parties.erase(id);
        }
        else
        {
            BroadcastPartyInfo(id);
        }

        pPartyComponent->JoinedPartyId.reset();
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

            playerList.Players[apPlayer->GetId()] = apPlayer->GetUsername();
        }

        GameServer::Get()->Send(apPlayer->GetConnectionId(), playerList);
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
    for (auto pPlayer : members)
    {
        message.PlayerIds.push_back(pPlayer->GetId());
    }

    for (auto pPlayer : members)
    {
        pPlayer->Send(message);
    }
}
