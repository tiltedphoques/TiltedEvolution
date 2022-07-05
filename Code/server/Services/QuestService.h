#pragma once

#include <Events/PacketEvent.h>
#include <Structs/GameId.h>

struct World;
struct UpdateEvent;
struct RequestQuestUpdate;
struct TopicRequest;

/**
* @brief Dispatch quest sync messages.
* 
* This service is currently not in use.
*/
class QuestService
{
public:
    QuestService(World& aWorld, entt::dispatcher& aDispatcher);

private:

    void OnQuestChanges(const PacketEvent<RequestQuestUpdate>& aChanges) noexcept;
    void OnTopicRequest(const PacketEvent<TopicRequest>& acRequest) noexcept;

    World& m_world;

    entt::scoped_connection m_questUpdateConnection;
    entt::scoped_connection m_topicConnection;
};
