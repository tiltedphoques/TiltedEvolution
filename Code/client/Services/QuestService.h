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
 * @brief Syncs quest starts, stops and stages across the party
 *
 * Every client runs the party's quests itself (scripts, scenes, dialogue), and only starts, stops and stages are
 * shared. Anything else a quest holds (aliases, script properties) only matches when the stage fragments produce it.
 *
 * Sending: TESQuestStartStopEvent and TESQuestStageEvent go out as RequestQuestUpdate, only in a party (outside one, an
 * update may prompt the overlay to suggest one). Quests without stages or in kNonSyncableQuestIds are never sent, and
 * the server drops None/Miscellaneous ones unless bEnableMiscQuestSync is set.
 *
 * Relaying: the server sends updates to the rest of the party as NotifyQuestUpdate. Members' scripts often reach the
 * same stage on their own, so it relays each once, from whoever reaches it first, until the quest next starts or stops.
 *
 * Applying: stages go through ScriptSetStage so their fragments run (and a stopped quest restarts), skipping those
 * already reached. Stops skip scripts. ScopedQuestOverride keeps the events this fires from being sent back.
 *
 * Scenes: setting a stage a playing scene waits on can end its phase early, skipping its lines and scripts and leaving
 * the quest stuck. So the leader, whose world the party follows, holds member stages that an upcoming phase checks and
 * queues later ones behind them. Members apply every stage right away.
 */
class QuestService final : public BSTEventSink<TESQuestStartStopEvent>,
                           BSTEventSink<TESQuestStageEvent>,
                           BSTEventSink<TESQuestStageItemDoneEvent>,
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
    BSTEventResult OnEvent(const TESQuestStageItemDoneEvent*, const EventDispatcher<TESQuestStageItemDoneEvent>*) override;
    BSTEventResult OnEvent(const TESSceneEvent*, const EventDispatcher<TESSceneEvent>*) override;
    BSTEventResult OnEvent(const TESSceneActionEvent*, const EventDispatcher<TESSceneActionEvent>*) override;
    BSTEventResult OnEvent(const TESScenePhaseEvent*, const EventDispatcher<TESScenePhaseEvent>*) override;
    BSTEventResult OnEvent(const TESTopicInfoEvent*, const EventDispatcher<TESTopicInfoEvent>*) override;

    struct HeldStages
    {
        // Member stages not applied yet, sorted
        Vector<uint16_t> Stages{};
        // A stage per log entry of the applied stages whose fragment hasn't run yet
        Vector<uint16_t> PendingItems{};
    };

    void OnQuestUpdate(const NotifyQuestUpdate&) noexcept;
    void HoldStage(TESQuest* apQuest, uint16_t aStage) noexcept;
    void ApplyHeldStages(TESQuest* apQuest, HeldStages& aHeld, size_t aCount) noexcept;
    // The quest's held stages and the quest, while it has stages to apply or fragments to wait for. Drops the stages
    // that can't be applied anymore.
    HeldStages* GetHeldStages(uint32_t aQuestFormId, TESQuest*& apQuest) noexcept;
    void ReleaseStagesForWaitingPhases(TESQuest* apQuest, HeldStages& aHeld) noexcept;
    void ReleaseUncheckedStages(TESQuest* apQuest, HeldStages& aHeld) noexcept;
    void NotifyOverlayOfQuestUpdate(uint32_t aFormId) noexcept;

    World& m_world;
    // Keyed by quest form ID
    Map<uint32_t, HeldStages> m_heldStages{};

    entt::scoped_connection m_joinedConnection;
    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_disconnectedConnection;
    entt::scoped_connection m_leftConnection;
    entt::scoped_connection m_questUpdateConnection;
};
