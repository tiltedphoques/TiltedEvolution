#pragma once

#include <Events/PacketEvent.h>

struct World;
struct EnterWorldSpaceRequest;
struct EnterCellRequest;

struct PlayerService
{
    PlayerService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~PlayerService() noexcept = default;

    TP_NOCOPYMOVE(PlayerService);

protected:

    void HandleWorldSpaceEnter(const PacketEvent<EnterWorldSpaceRequest>& acMessage) const noexcept;
    void HandleCellEnter(const PacketEvent<EnterCellRequest>& acMessage) const noexcept;

private:

    World& m_world;

    entt::scoped_connection m_cellEnterConnection;
};
