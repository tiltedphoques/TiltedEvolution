#include <Services/DebugService.h>

#include <Messages/PartyKickRequest.h>
#include <Messages/PartyChangeLeaderRequest.h>
#include <Messages/PartyInviteRequest.h>
#include <Messages/PartyAcceptInviteRequest.h>
#include <Messages/PartyLeaveRequest.h>
#include <Messages/PartyCreateRequest.h>
#include <Messages/TeleportCommandRequest.h>

#include <World.h>

#include <imgui.h>

void DebugService::DrawPartyView()
{
    if (!m_transport.IsConnected())
        return;

    ImGui::Begin("Party");

    auto& partyService = m_world.GetPartyService();
    auto& players = partyService.GetPlayers();
    auto& members = partyService.GetPartyMembers();
    auto& invitations = partyService.GetInvitations();

    if (partyService.IsInParty())
    {
        ImGui::Text("Party Members");
        String pName{"You"};
        if (partyService.IsLeader())
        {
            pName += " (Leader)";
        }
        ImGui::BulletText(pName.c_str());

        for (auto& playerId : members)
        {
            ImGui::PushID(playerId);

            auto playerEntry = players.find(playerId);
            if (playerEntry != players.end())
            {
                auto playerName = playerEntry.value();
                if (playerId == partyService.GetLeaderPlayerId())
                {
                    playerName += " (Leader)";
                }
                ImGui::BulletText(playerName.c_str());

                ImGui::SameLine(200);
                if (ImGui::Button("Teleport"))
                {
                    TeleportCommandRequest request{};
                    request.TargetPlayer = playerEntry.value();

                    m_transport.Send(request);
                }

                if (partyService.IsLeader())
                {
                    ImGui::SameLine();
                    if (ImGui::Button("Kick"))
                    {
                        PartyKickRequest kickMessage;
                        kickMessage.PartyMemberPlayerId = playerEntry.key();
                        m_transport.Send(kickMessage);
                    }

                    ImGui::SameLine();
                    if (ImGui::Button("Make Leader"))
                    {
                        PartyChangeLeaderRequest changeMessage;
                        changeMessage.PartyMemberPlayerId = playerEntry.key();
                        m_transport.Send(changeMessage);
                    }
                }
            }
            
            ImGui::PopID();
        }

        if (partyService.IsLeader())
        {
            ImGui::NewLine();
            ImGui::Text("Other Players");
            auto playerCount = 0;
            for (auto& player : partyService.GetPlayers())
            {
                if (std::find(std::begin(members), std::end(members), player.first) !=
                    std::end(members))
                    continue;
                
                playerCount++;
                ImGui::BulletText(player.second.c_str());
                ImGui::SameLine(100);
                if (ImGui::Button("Invite"))
                {
                    PartyInviteRequest request;
                    request.PlayerId = player.first;
                    m_transport.Send(request);
                }
            }
            
            if (playerCount == 0)
            {
                ImGui::BulletText("<No one online>");
            }
        }
    }

    for (auto& player : players)
    {
        auto itor = invitations.find(player.first);
        if (itor != std::end(invitations))
        {
            if (std::find(std::begin(members), std::end(members), player.first) !=
                std::end(members))
                continue;

            ImGui::Text(player.second.c_str());
            ImGui::SameLine(100);
            if (ImGui::Button("Accept"))
            {
                invitations.erase(itor);

                PartyAcceptInviteRequest message;
                message.InviterId = player.first;
                m_transport.Send(message);
            }
        }
    }

    ImGui::NewLine();
    if (partyService.IsInParty())
    {
        if (ImGui::Button("Leave"))
        {
            PartyLeaveRequest request;
            m_transport.Send(request);
        }
    }
    else
    {
        if (ImGui::Button("Create Party"))
        {
            PartyCreateRequest request;
            m_transport.Send(request);
        }
    }

    ImGui::End();
}

