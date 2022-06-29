#pragma once

#include <Messages/SendChatMessageRequest.h>

#include <Events/PacketEvent.h>
#include <Events/PlayerEnterWorldEvent.h>

struct World;

struct PlayerDialogueRequest;
struct TeleportRequest;
struct RequestPlayerHealthUpdate;

/**
* @brief Dispatches UI events that modify the UI view of other cients.
*/
class OverlayService
{
  public:
    OverlayService(World& aWorld, entt::dispatcher& aDispatcher);

  protected:
    /**
    * @brief Applies regex on chat message and relays it to other clients.
    */
    void HandleChatMessage(const PacketEvent<SendChatMessageRequest>& acMessage) const noexcept;
    void HandlePlayerJoin(const PlayerEnterWorldEvent& acEvent) const noexcept;
    void OnPlayerDialogue(const PacketEvent<PlayerDialogueRequest>& acMessage) const noexcept;
    void OnTeleport(const PacketEvent<TeleportRequest>& acMessage) const noexcept;
    void OnPlayerHealthUpdate(const PacketEvent<RequestPlayerHealthUpdate>& acMessage) const noexcept;

  private:
    World& m_world;

    entt::scoped_connection m_chatMessageConnection;
    entt::scoped_connection m_playerDialogueConnection;
    entt::scoped_connection m_teleportConnection;
    entt::scoped_connection m_playerHealthConnection;
};
