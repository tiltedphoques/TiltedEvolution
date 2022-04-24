#pragma once
#include <Scripts/Player.h>

#include <Messages/SendChatMessageRequest.h>

#include <Events/PacketEvent.h>
#include <Events/PlayerEnterWorldEvent.h>

struct World;

/**
* @brief Dispatches UI events that modify the UI view of other cients.
*/
class OverlayService
{
  public:
    OverlayService(World& aWorld, entt::dispatcher& aDispatcher);

    void BroadcastMessage(const std::string aMessage);

  protected:
    void HandleChatMessage(const PacketEvent<SendChatMessageRequest>& acMessage) const noexcept;
    void HandlePlayerJoin(const PlayerEnterWorldEvent& acEvent) const noexcept;

  private:
    World& m_world;

    entt::scoped_connection m_chatMessageConnection;
    entt::scoped_connection m_playerEnterWorldConnection;
};
