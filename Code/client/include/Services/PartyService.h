#pragma once

struct ImguiService;
struct TransportService;
struct NotifyPlayerList;
struct NotifyPartyInfo;
struct NotifyPartyInvite;
struct UpdateEvent;

struct PartyService
{
    PartyService(entt::dispatcher& aDispatcher, ImguiService& aImguiService, TransportService& aTransportService) noexcept;
    ~PartyService() = default;

    TP_NOCOPYMOVE(PartyService);

protected:

    void OnUpdate(const UpdateEvent& acPlayerList) noexcept;
    void OnPlayerList(const NotifyPlayerList& acPlayerList) noexcept;
    void OnPartyInfo(const NotifyPartyInfo& acPartyInfo) noexcept;
    void OnPartyInvite(const NotifyPartyInvite& acPartyInvite) noexcept;

private:
    void OnDraw() noexcept;

    Map<uint32_t, String> m_players;
    Vector<uint32_t> m_partyMembers;
    Map<uint32_t, uint64_t> m_invitations;
    uint64_t m_nextUpdate{0};

    TransportService& m_transportService;

    entt::scoped_connection m_drawConnection;
    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_playerListConnection;
    entt::scoped_connection m_partyInfoConnection;
    entt::scoped_connection m_partyInviteConnection;
};
