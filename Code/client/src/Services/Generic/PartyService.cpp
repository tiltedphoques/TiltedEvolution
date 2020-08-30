
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

PartyService::PartyService(entt::dispatcher& aDispatcher, ImguiService& aImguiService, TransportService& aTransportService) noexcept
    : m_transportService(aTransportService)
{
    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&PartyService::OnUpdate>(this);
    m_drawConnection = aImguiService.OnDraw.connect<&PartyService::OnDraw>(this);

    m_playerListConnection = aDispatcher.sink<NotifyPlayerList>().connect<&PartyService::OnPlayerList>(this);
    m_partyInfoConnection = aDispatcher.sink<NotifyPartyInfo>().connect<&PartyService::OnPartyInfo>(this);
    m_partyInviteConnection = aDispatcher.sink<NotifyPartyInvite>().connect<&PartyService::OnPartyInvite>(this);
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

void PartyService::OnPartyInfo(const NotifyPartyInfo& acPlayerList) noexcept
{
    m_partyMembers = acPlayerList.PlayerIds;
}

void PartyService::OnPartyInvite(const NotifyPartyInvite& acPartyInvite) noexcept
{
    m_invitations[acPartyInvite.InviterId] = m_transportService.GetClock().GetCurrentTick() + 60000;
}

void PartyService::OnDraw() noexcept
{
    if (!m_transportService.IsConnected())
        return;

    ImGui::Begin("Party");

    for (auto& player : m_players)
    {
        ImGui::Text(player.second.c_str());

        if (std::find(std::begin(m_partyMembers), std::end(m_partyMembers), player.first) != std::end(m_partyMembers))
            continue;

        ImGui::SameLine(100);

        auto itor = m_invitations.find(player.first);
        if (itor != std::end(m_invitations))
        {
            if (ImGui::Button("Accept"))
            {
                m_invitations.erase(itor);

                PartyAcceptInviteRequest message;
                message.InviterId = player.first;

                m_transportService.Send(message);
            }
        }
        else if(ImGui::Button("Invite"))
        {
            PartyInviteRequest request;
            request.PlayerId = player.first;

            m_transportService.Send(request);
        }
    }

    if (ImGui::Button("Leave"))
    {
        PartyLeaveRequest request;
        m_transportService.Send(request);
    }

    ImGui::End();
}
