#pragma once

#include <World.h>
#include <Events/EventDispatcher.h>
#include <Games/Events.h>

struct NotifyQuestUpdate;
struct NotifyQuestSceneUpdate;

struct TESQuest;

/**
 * @brief Handles quest sync
 */
class QuestService final : public BSTEventSink<TESQuestStartStopEvent>, BSTEventSink<TESQuestStageEvent>, BSTEventSink<TESSceneEvent>, BSTEventSink<TESSceneActionEvent>, BSTEventSink<TESScenePhaseEvent>
{
public:
    QuestService(World&, entt::dispatcher&);
    ~QuestService() = default;

    static bool IsNonSyncableQuest(TESQuest* apQuest);
    static void DebugDumpQuests();
    static bool StopQuest(uint32_t aformId);
    const uint32_t PlayerId() const noexcept { return m_playerId; }

private:
    friend struct QuestEventHandler;

    void OnConnected(const ConnectedEvent&) noexcept;
    void Disconnected(const DisconnectedEvent&) noexcept { m_playerId = 0; }


    BSTEventResult OnEvent(const TESQuestStartStopEvent*, const EventDispatcher<TESQuestStartStopEvent>*) override;
    BSTEventResult OnEvent(const TESQuestStageEvent*, const EventDispatcher<TESQuestStageEvent>*) override;
    BSTEventResult OnEvent(const TESSceneEvent*, const EventDispatcher<TESSceneEvent>*) override;
#if 1
    BSTEventResult OnEvent(const TESSceneActionEvent*, const EventDispatcher<TESSceneActionEvent>*) override;
    BSTEventResult OnEvent(const TESScenePhaseEvent*, const EventDispatcher<TESScenePhaseEvent>*) override;
#endif
    void OnQuestUpdate(const NotifyQuestUpdate&) noexcept;
    void OnQuestSceneUpdate(const NotifyQuestSceneUpdate&) noexcept;

    bool CanAdvanceQuestForParty() const noexcept;

    World& m_world;
    uint32_t m_playerId;

    entt::scoped_connection m_joinedConnection;
    entt::scoped_connection m_leftConnection;
    entt::scoped_connection m_questUpdateConnection;
    entt::scoped_connection m_questSceneUpdateConnection;
};
