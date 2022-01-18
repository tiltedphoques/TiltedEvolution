#include <TiltedOnlinePCH.h>


#include <imgui.h>
#include <Events/UpdateEvent.h>

#include <Services/PartyService.h>
#include <Services/ImguiService.h>
#include <Services/TransportService.h>

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

PartyService::PartyService(entt::dispatcher& aDispatcher, ImguiService& aImguiService, TransportService& aTransportService) noexcept
    : m_transportService(aTransportService)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&PartyService::OnUpdate>(this);
    m_drawConnection = aImguiService.OnDraw.connect<&PartyService::OnDraw>(this);

    m_playerListConnection = aDispatcher.sink<NotifyPlayerList>().connect<&PartyService::OnPlayerList>(this);
    m_partyInfoConnection = aDispatcher.sink<NotifyPartyInfo>().connect<&PartyService::OnPartyInfo>(this);
    m_partyInviteConnection = aDispatcher.sink<NotifyPartyInvite>().connect<&PartyService::OnPartyInvite>(this);
    m_partyJoinedConnection = aDispatcher.sink<NotifyPartyJoined>().connect<&PartyService::OnPartyJoined>(this);
    m_partyLeftConnection = aDispatcher.sink<NotifyPartyLeft>().connect<&PartyService::OnPartyLeft>(this);
}

void PartyService::OnUpdate(const UpdateEvent& acEvent) noexcept
{
    const auto cCurrentTick = m_transportService.GetClock().GetCurrentTick();
    if (m_nextUpdate > cCurrentTick)
        return;

    // Update once every second
    m_nextUpdate = cCurrentTick + 1000;

    auto itor = std::begin(m_invitations);
    while (itor != std::end(m_invitations))
    {
        if (itor->second < cCurrentTick)
        {
            itor = m_invitations.erase(itor);
        }
        else
        {
            ++itor;
        }
    }
}

void PartyService::OnPlayerList(const NotifyPlayerList& acPlayerList) noexcept
{
    m_players = acPlayerList.Players;
}

void PartyService::OnPartyInfo(const NotifyPartyInfo& acPartyInfo) noexcept
{
    if (m_inParty)
    {
        spdlog::debug("[PartyService]: Got party info update");
        m_isLeader = acPartyInfo.IsLeader;
        m_leaderPlayerId = acPartyInfo.LeaderPlayerId;
        m_partyMembers = acPartyInfo.PlayerIds;
    }
}

void PartyService::OnPartyInvite(const NotifyPartyInvite& acPartyInvite) noexcept
{
    spdlog::debug("[PartyService]: Got party invite from {}", acPartyInvite.InviterId);
    m_invitations[acPartyInvite.InviterId] = acPartyInvite.ExpiryTick;
}

void PartyService::OnPartyLeft(const NotifyPartyLeft& acPartyLeft) noexcept
{
    spdlog::debug("[PartyService]: Left party");
    m_inParty = false;
    m_isLeader = false;
    m_leaderPlayerId = -1;
    m_partyMembers.clear();
}

void PartyService::OnPartyJoined(const NotifyPartyJoined& acPartyJoined) noexcept
{
    spdlog::debug("[PartyService]: Joined party. LeaderId: {}, IsLeader: {}", acPartyJoined.LeaderPlayerId,
                  acPartyJoined.IsLeader);
    m_inParty = true;
    m_isLeader = acPartyJoined.IsLeader;
    m_leaderPlayerId = acPartyJoined.LeaderPlayerId;
    m_partyMembers = acPartyJoined.PlayerIds;
}

void PartyService::OnDraw() noexcept
{
    if (!m_transportService.IsConnected())
        return;

    ImGui::Begin("Party");

    if (m_inParty)
    {
        ImGui::Text("Party Members");
        String pName{"You"};
        if (m_isLeader)
        {
            pName += " (Leader)";
        }
        ImGui::BulletText(pName.c_str());

        for (auto& playerId : m_partyMembers)
        {
            auto playerEntry = m_players.find(playerId);
            if (playerEntry != m_players.end())
            {
                auto playerName = playerEntry.value();
                if (playerId == m_leaderPlayerId)
                {
                    playerName += " (Leader)";
                }
                ImGui::BulletText(playerName.c_str());
                if (m_isLeader)
                {
                    ImGui::SameLine(100);
                    if (ImGui::Button("Kick"))
                    {
                        PartyKickRequest kickMessage;
                        kickMessage.PartyMemberPlayerId = playerEntry.key();
                        m_transportService.Send(kickMessage);
                    }

                    ImGui::SameLine(150);
                    if (ImGui::Button("Make Leader"))
                    {
                        PartyChangeLeaderRequest changeMessage;
                        changeMessage.PartyMemberPlayerId = playerEntry.key();
                        m_transportService.Send(changeMessage);
                    }
                }
            }
            
        }

        if (m_isLeader)
        {
            ImGui::NewLine();
            ImGui::Text("Other Players");
            auto playerCount = 0;
            for (auto& player : m_players)
            {
                if (std::find(std::begin(m_partyMembers), std::end(m_partyMembers), player.first) !=
                    std::end(m_partyMembers))
                    continue;
                
                playerCount++;
                ImGui::BulletText(player.second.c_str());
                ImGui::SameLine(100);
                if (ImGui::Button("Invite"))
                {
                    PartyInviteRequest request;
                    request.PlayerId = player.first;
                    m_transportService.Send(request);
                }
            }
            
            if (playerCount == 0)
            {
                ImGui::BulletText("<No one online>");
            }
        }
    }

    for (auto& player : m_players)
    {
        auto itor = m_invitations.find(player.first);
        if (itor != std::end(m_invitations))
        {
            if (std::find(std::begin(m_partyMembers), std::end(m_partyMembers), player.first) !=
                std::end(m_partyMembers))
                continue;

            ImGui::Text(player.second.c_str());
            ImGui::SameLine(100);
            if (ImGui::Button("Accept"))
            {
                m_invitations.erase(itor);

                PartyAcceptInviteRequest message;
                message.InviterId = player.first;
                m_transportService.Send(message);
            }
        }
    }

    ImGui::NewLine();
    if (m_inParty)
    {
        if (ImGui::Button("Leave"))
        {
            PartyLeaveRequest request;
            m_transportService.Send(request);
        }
    }
    else
    {
        if (ImGui::Button("Create Party"))
        {
            PartyCreateRequest request;
            m_transportService.Send(request);
        }
    }

    ImGui::End();
}
