#pragma once

#include <World.h>
#include <Events/EventDispatcher.h>
#include <Games/Events.h>

struct TopicEvent;

struct NotifyQuestUpdate;
struct NotifyTopic;

struct TESQuest;

/**
* @brief Handles quest sync
* 
* This service is currently not in use.
*/
class QuestService final : public 
    BSTEventSink<TESQuestStartStopEvent>, 
    BSTEventSink<TESQuestStageEvent>
{
public:
    QuestService(World&, entt::dispatcher&);
    ~QuestService() = default;

    static bool IsNonSyncableQuest(TESQuest* apQuest);
    static void DebugDumpQuests();
    static TESQuest* SetQuestStage(uint32_t aformId, uint16_t aStage);
    static bool StopQuest(uint32_t aformId);

private:
    void OnConnected(const ConnectedEvent&) noexcept;
    void OnTopicEvent(const TopicEvent&) noexcept;

    BSTEventResult OnEvent(const TESQuestStartStopEvent*, const EventDispatcher<TESQuestStartStopEvent>*) override;
    BSTEventResult OnEvent(const TESQuestStageEvent*, const EventDispatcher<TESQuestStageEvent>*) override;

    void OnQuestUpdate(const NotifyQuestUpdate&) noexcept;
    void OnNotifyTopic(const NotifyTopic&) noexcept;

    World& m_world;

    entt::scoped_connection m_joinedConnection;
    entt::scoped_connection m_leftConnection;
    entt::scoped_connection m_questUpdateConnection;
    entt::scoped_connection m_topicConnection;
    entt::scoped_connection m_topicNotifyConnection;
};
