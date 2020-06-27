#include <Services/PlayerService.h>
#include <Services/CharacterService.h>
#include <Components.h>
#include <GameServer.h>

PlayerService::PlayerService(World& aWorld, entt::dispatcher& aDispatcher) noexcept
    : m_world(aWorld)
    , m_cellEnterConnection(aDispatcher.sink<PacketEvent<TiltedMessages::CellEnterRequest>>().connect<&PlayerService::HandleCellEnter>(this))
{
}

void PlayerService::HandleCellEnter(const PacketEvent<TiltedMessages::CellEnterRequest>& acMessage) noexcept
{
    auto playerView = m_world.view<PlayerComponent>();

    const auto itor = std::find_if(std::begin(playerView), std::end(playerView),
   [playerView, connectionId = acMessage.ConnectionId](auto entity)
    {
        return playerView.get(entity).ConnectionId == connectionId;
    });

    if(itor == std::end(playerView))
    {
        spdlog::error("Connection {:x} is not associated with a player.", acMessage.ConnectionId);
        return;
    }

    m_world.assign_or_replace<CellIdComponent>(*itor, acMessage.Packet.cell_id());

    auto characterView = m_world.view<CellIdComponent, CharacterComponent, OwnerComponent>();
    for (auto character : characterView)
    {
        const auto& ownedComponent = characterView.get<OwnerComponent>(character);

        // Don't send self managed
        if (ownedComponent.ConnectionId == acMessage.ConnectionId)
            continue;

        TiltedMessages::ServerMessage message;
        const auto pRequest = message.mutable_character_spawn_request();
        CharacterService::Serialize(m_world, character, pRequest);

        //GameServer::Get()->Send(acMessage.ConnectionId, message);
    }
}
