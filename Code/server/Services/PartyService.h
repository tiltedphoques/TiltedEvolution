#pragma once

#include <Events/PacketEvent.h>

struct World;
struct UpdateEvent;
struct PlayerJoinEvent;
struct PlayerLeaveEvent;
struct PartyInviteRequest;
struct PartyAcceptInviteRequest;
struct PartyLeaveRequest;
struct NotifyPartyInfo;
struct PartyCreateRequest;
struct PartyChangeLeaderRequest;
struct PartyKickRequest;

/*  With the introduction of party member (not just leader) quest progression, there are a few challenges:
    SendToParty() will cause reflections of the same quest updates from all party members.
    This is because ScopedQuestOverride() doesn't work as intended because QuestService::OnEvent calls
    sometimes happen later on a different thread. We need to prevent these reflections from sending out dups.

    We want the quest progress to be sent out by the Leader. It has already happened for one Member when
    members advance the quest and it is theoretically possible more than one sends progress, so SendToParty
    needs to not send to those who already have it.

    Since we have to track this anyway, if a Member advances quest, we forward it to the Leader for broadcast.
    This means Leader gets the update in QuestService::OnUpdate(), and when the Leader update reflects back, the
    Leader does SendToParty() (with the original Member and the Leadre in the cache, so SendToParty doesn't send
    to them). This enables the Leader to make a centralized decision; it can reject sending out a member update
    that shouldn't be forwarded; it looks like there are a couple of exceptions like that.
*/
struct QuestStageDedupHistory
{
    using QuestOrSceneId = GameId;
    using QuestStage = uint16_t;
    using QuestStatus = uint8_t;
    using PlayerId = uint32_t;
    using TimeStamp = std::chrono::time_point<std::chrono::steady_clock>;

    struct Entry
    {
        QuestOrSceneId questOrSceneId{};
        QuestStage questStage{};
        QuestStatus questStatus{};
        PlayerId playerId{};
        TimeStamp timestamp{};
    };

    using Container = std::deque<Entry>;
    using iterator = Container::iterator;
    using const_iterator = Container::const_iterator;

    void Add(const QuestOrSceneId& acQuestOrSceneId, const QuestStage acQuestStage, const QuestStatus acQuestStatus,
             const PlayerId& acPlayerId, const TimeStamp acTimeStamp = std::chrono::steady_clock::now());

    const_iterator Find(const QuestOrSceneId& acQuestOrSceneId, const QuestStage acQuestStage,
                        const QuestStatus acQuestStatus) noexcept;
    const_iterator FindWPlayerId(const QuestOrSceneId& acQuestOrSceneId, const QuestStage acQuestStage,
                                 const QuestStatus acQuestStatus, const PlayerId& acPlayerId) noexcept;

    void Reset() noexcept { m_cache.clear(); }
    bool Found(const QuestOrSceneId& acQuestOrSceneId, const QuestStage acQuestStage,
               const QuestStatus acQuestStatus) noexcept
    {
        return Find(acQuestOrSceneId, acQuestStage, acQuestStatus) != m_cache.end();
    }
    bool FoundWPlayerId(const QuestOrSceneId& acQuestOrSceneId, const QuestStage acQuestStage,
                        const QuestStatus acQuestStatus, const PlayerId& acPlayerId) noexcept
    {
        return FindWPlayerId(acQuestOrSceneId, acQuestStage, acQuestStatus, acPlayerId) != m_cache.end();
    }

    uint32_t GetSceneMaster() const noexcept { return m_sceneMaster; }
    void SetSceneMaster(const uint32_t acSceneMaster) noexcept { m_sceneMaster = acSceneMaster; }
    void ResetSceneMaster() noexcept { m_sceneMaster = 0; }
    const bool NeedSceneMaster() const noexcept { return !m_sceneMaster; }

  private:
    void inline Expire() noexcept;
    Container m_cache{};       // Short, time-ordered, duplicates valid (do they ever happen?)
    uint32_t m_sceneMaster{0}; // PlayerIDs start at 1, so zero is "not set."
};

/**
 * @brief Manages every party in the server.
 */
struct PartyService
{
    struct Party
    {
        uint32_t LeaderPlayerId;
        Vector<Player*> Members;
        GameId CachedWeather{};
        QuestStageDedupHistory m_questStageDedupHistory;
        QuestStageDedupHistory& GetQuestStageDedupHistory() { return m_questStageDedupHistory; }
    };

    PartyService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~PartyService() noexcept = default;

    TP_NOCOPYMOVE(PartyService);

    const Party* GetById(uint32_t aId) const noexcept;
    bool IsPlayerInParty(Player* const apPlayer) const noexcept;
    bool IsPlayerLeader(const Player* const apPlayer) const noexcept;
    Party* GetPlayerParty(Player* const apPlayer) noexcept;

protected:
    void OnUpdate(const UpdateEvent& acEvent) noexcept;
    void OnPlayerJoin(const PlayerJoinEvent& acEvent) noexcept;
    void OnPlayerLeave(const PlayerLeaveEvent& acEvent) noexcept;
    void OnPartyInvite(const PacketEvent<PartyInviteRequest>& acPacket) noexcept;
    void OnPartyAcceptInvite(const PacketEvent<PartyAcceptInviteRequest>& acPacket) noexcept;
    void OnPartyLeave(const PacketEvent<PartyLeaveRequest>& acPacket) noexcept;
    void OnPartyCreate(const PacketEvent<PartyCreateRequest>& acPacket) noexcept;
    void OnPartyChangeLeader(const PacketEvent<PartyChangeLeaderRequest>& acPacket) noexcept;
    void OnPartyKick(const PacketEvent<PartyKickRequest>& acPacket) noexcept;
    void RemovePlayerFromParty(Player* apPlayer) noexcept;

    void BroadcastPlayerList(Player* apPlayer = nullptr) const noexcept;
    void BroadcastPartyInfo(uint32_t aPartyId) const noexcept;

private:
    World& m_world;

    TiltedPhoques::Map<uint32_t, Party> m_parties;
    uint32_t m_nextId{0};
    uint64_t m_nextInvitationExpire{0};

    entt::scoped_connection m_updateEvent;
    entt::scoped_connection m_playerJoinConnection;
    entt::scoped_connection m_playerLeaveConnection;
    entt::scoped_connection m_partyInviteConnection;
    entt::scoped_connection m_partyAcceptInviteConnection;
    entt::scoped_connection m_partyLeaveConnection;
    entt::scoped_connection m_partyCreateConnection;
    entt::scoped_connection m_partyChangeLeaderConnection;
    entt::scoped_connection m_partyKickConnection;

    void SendPartyJoinedEvent(Party& aParty, Player* aPlayer) noexcept;
};
