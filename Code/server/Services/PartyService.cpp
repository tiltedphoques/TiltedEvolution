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
    // When a player joins give it a party component for later
    m_world.emplace<PartyComponent>(acEvent.Entity);

    BroadcastPlayerList();
}

void PartyService::OnPartyInvite(const PacketEvent<PartyInviteRequest>& acPacket) const noexcept
{
    auto& message = acPacket.Packet;

    const entt::entity entity{message.PlayerId};

    auto view = m_world.view<PlayerComponent, PartyComponent>();

    // Make sure the player we invite exists
    const auto otherItor = view.find(entity);

    // Get self
    const auto selfItor =
        std::find_if(view.begin(), view.end(), [view, connectionId = acPacket.ConnectionId](auto entity) {
            return view.get<PlayerComponent>(entity).ConnectionId == connectionId;
        });

    // If both players are available and they are different
    if (otherItor != view.end() && selfItor != view.end() && *otherItor != *selfItor)
    {
        auto& otherPartyComponent = view.get<PartyComponent>(*otherItor);

        // Expire in 60 seconds
        const auto cExpiryTick = GameServer::Get()->GetTick() + 60000;
        otherPartyComponent.Invitations[*selfItor] = cExpiryTick;

        NotifyPartyInvite notification;
        notification.InviterId = World::ToInteger(*selfItor);
        notification.ExpiryTick = cExpiryTick;

        auto& playerComponent = view.get<PlayerComponent>(*otherItor);

        GameServer::Get()->Send(playerComponent.ConnectionId, notification);
    }
}

void PartyService::OnPartyAcceptInvite(const PacketEvent<PartyAcceptInviteRequest>& acPacket) noexcept
{
    auto& message = acPacket.Packet;

    entt::entity entity{message.InviterId};

    auto view = m_world.view<PlayerComponent, PartyComponent>();

    // Make sure the player that accepted exists
    const auto selfItor = view.find(entity);

    // Get the inviter
    const auto inviterItor =
        std::find_if(view.begin(), view.end(), [view, connectionId = acPacket.ConnectionId](auto entity) {
            return view.get<PlayerComponent>(entity).ConnectionId == connectionId;
        });

    // If both players are available and they are different
    if (inviterItor != view.end() && selfItor != view.end() && *inviterItor != *selfItor)
    {
        auto& inviterPartyComponent = view.get<PartyComponent>(*inviterItor);
        auto& selfPartyComponent = view.get<PartyComponent>(*selfItor);

        auto partyId = 0;

        // Check if we have this invitation so people don't invite themselves
        if (selfPartyComponent.Invitations.count(*inviterItor) == 0)
            return;

        // If the inviter isn't in a party, create it
        if (!inviterPartyComponent.JoinedPartyId)
        {
            partyId = m_nextId++;

            auto& party = m_parties[partyId];
            party.Members.push_back(*inviterItor);
            inviterPartyComponent.JoinedPartyId = partyId;
        }
        else
        {
            partyId = *inviterPartyComponent.JoinedPartyId;
        }

        auto& party = m_parties[partyId];
        party.Members.push_back(*selfItor);
        selfPartyComponent.JoinedPartyId = partyId;

        BroadcastPartyInfo(partyId);
    }
}

void PartyService::OnPartyLeave(const PacketEvent<PartyLeaveRequest>& acPacket) noexcept
{
    auto& message = acPacket.Packet;

    auto view = m_world.view<PlayerComponent, PartyComponent>();

    // Get self
    const auto selfItor =
        std::find_if(view.begin(), view.end(), [view, connectionId = acPacket.ConnectionId](auto entity) {
            return view.get<PlayerComponent>(entity).ConnectionId == connectionId;
        });

    if (selfItor != view.end())
    {
        RemovePlayerFromParty(*selfItor);
    }
}

void PartyService::OnPlayerLeave(const PlayerLeaveEvent& acEvent) noexcept
{
    RemovePlayerFromParty(acEvent.Entity);

    BroadcastPlayerList(acEvent.Entity);
}

void PartyService::RemovePlayerFromParty(entt::entity aEntity) noexcept
{
    auto* pPartyComponent = m_world.try_get<PartyComponent>(aEntity);
    if (!pPartyComponent)
        return;

    if (pPartyComponent->JoinedPartyId)
    {
        auto id = *pPartyComponent->JoinedPartyId;

        auto& party = m_parties[id];
        auto& members = party.Members;

        members.erase(std::find(std::begin(members), std::end(members), aEntity));

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

void PartyService::BroadcastPlayerList(std::optional<entt::entity> aSkipEntity) const noexcept
{
    auto playerView = m_world.view<const PlayerComponent>();

    for (auto player : playerView)
    {
        if (aSkipEntity && *aSkipEntity == player)
            continue;

        NotifyPlayerList playerList;

        auto& currentPlayerComponent = playerView.get<const PlayerComponent>(player);

        for (auto otherPlayer : playerView)
        {
            if (player == otherPlayer)
                continue;

            if (aSkipEntity && *aSkipEntity == otherPlayer)
                continue;

            auto& otherPlayerComponent = playerView.get<const PlayerComponent>(otherPlayer);

            playerList.Players[World::ToInteger(otherPlayer)] = otherPlayerComponent.Username;
        }

        GameServer::Get()->Send(currentPlayerComponent.ConnectionId, playerList);
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
    for (auto entity : members)
    {
        message.PlayerIds.push_back(World::ToInteger(entity));
    }

    for (auto entity : members)
    {
        auto* pPlayerComponent = m_world.try_get<PlayerComponent>(entity);
        if (!pPlayerComponent)
            continue;

        GameServer::Get()->Send(pPlayerComponent->ConnectionId, message);
    }
}
