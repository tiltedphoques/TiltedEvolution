#pragma once

#include <Events/PacketEvent.h>

struct World;

struct PlayerService
{
    PlayerService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~PlayerService() noexcept = default;

    TP_NOCOPYMOVE(PlayerService);

protected:

    void HandleCellEnter(const PacketEvent<TiltedMessages::CellEnterRequest>& acMessage) noexcept;

private:

    World& m_world;

    entt::scoped_connection m_cellEnterConnection;
};
