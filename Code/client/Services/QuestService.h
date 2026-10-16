#pragma once

#include <World.h>
#include <Events/EventDispatcher.h>
#include <Games/Events.h>

struct NotifyQuestUpdate;
struct DisconnectedEvent;
struct PartyLeftEvent;
struct UpdateEvent;

struct TESQuest;

/**
 * @brief Handles quest sync
 *
 * Sends local quest starts, stops and stages to the party and applies theirs. The leader holds member stages while
 * a scene of their quest plays.
 */
class QuestService final : public BSTEventSink<TESQuestStartStopEvent>,
                           BSTEventSink<TESQuestStageEvent>,
                           BSTEventSink<TESSceneEvent>,
                           BSTEventSink<TESSceneActionEvent>,
                           BSTEventSink<TESScenePhaseEvent>,
                           BSTEventSink<TESTopicInfoEvent>
{
public:
    QuestService(World&, entt::dispatcher&);
    ~QuestService() = default;

    static bool IsNonSyncableQuest(TESQuest* apQuest);
    static void DebugDumpQuests();
    static bool StopQuest(uint32_t aformId);

private:
    void OnConnected(const ConnectedEvent&) noexcept;
    void OnUpdate(const UpdateEvent&) noexcept;
    void OnDisconnected(const DisconnectedEvent&) noexcept;
    void OnPartyLeft(const PartyLeftEvent&) noexcept;

    BSTEventResult OnEvent(const TESQuestStartStopEvent*, const EventDispatcher<TESQuestStartStopEvent>*) override;
    BSTEventResult OnEvent(const TESQuestStageEvent*, const EventDispatcher<TESQuestStageEvent>*) override;
    BSTEventResult OnEvent(const TESSceneEvent*, const EventDispatcher<TESSceneEvent>*) override;
    BSTEventResult OnEvent(const TESSceneActionEvent*, const EventDispatcher<TESSceneActionEvent>*) override;
    BSTEventResult OnEvent(const TESScenePhaseEvent*, const EventDispatcher<TESScenePhaseEvent>*) override;
    BSTEventResult OnEvent(const TESTopicInfoEvent*, const EventDispatcher<TESTopicInfoEvent>*) override;

    void OnQuestUpdate(const NotifyQuestUpdate&) noexcept;
    void HoldStage(TESQuest* apQuest, uint16_t aStage) noexcept;
    void ApplyHeldStages(TESQuest* apQuest, Vector<uint16_t>& aStages, size_t aCount) noexcept;
    // The quest's held stages and the quest, if it still has any to apply. Drops the ones that can't be anymore.
    Vector<uint16_t>* GetHeldStages(uint32_t aQuestFormId, TESQuest*& apQuest) noexcept;
    void ReleaseStagesForWaitingPhases(uint32_t aQuestFormId) noexcept;
    void ApplyHeldStagesAfterScene(uint32_t aQuestFormId, uint32_t aEndedSceneFormId) noexcept;
    void NotifyOverlayOfQuestUpdate(uint32_t aFormId) noexcept;

    World& m_world;
    // Member stages the leader holds while a scene of their quest plays, sorted, keyed by quest form ID
    Map<uint32_t, Vector<uint16_t>> m_heldStages{};

    entt::scoped_connection m_joinedConnection;
    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_disconnectedConnection;
    entt::scoped_connection m_leftConnection;
    entt::scoped_connection m_questUpdateConnection;
};
