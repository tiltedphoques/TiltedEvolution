#pragma once

#include <Events/PacketEvent.h>
#include <Structs/GameId.h>

struct World;
struct UpdateEvent;
struct RequestQuestUpdate;

class QuestService
{
public:
    QuestService(World& aWorld, entt::dispatcher& aDispatcher);

    bool StartStopQuest(Player* apRecipient, GameId aGameId, bool aStop) noexcept;
private:
    void HandleQuestChanges(const PacketEvent<RequestQuestUpdate>& aChanges) noexcept;

    entt::scoped_connection m_questUpdateConnection;
    entt::scoped_connection m_updateConnection;
    entt::scoped_connection m_joinConnection;
    World& m_world;
};
