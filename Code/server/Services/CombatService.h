#pragma once

#include <Events/PacketEvent.h>

struct World;
struct ProjectileLaunchRequest;

struct CombatService
{
    CombatService(World& aWorld, entt::dispatcher& aDispatcher) noexcept;
    ~CombatService() noexcept = default;

    TP_NOCOPYMOVE(CombatService);

protected:
    void OnProjectileLaunchRequest(const PacketEvent<ProjectileLaunchRequest>& acMessage) const noexcept;

private:

    World& m_world;

    entt::scoped_connection m_projectileLaunchConnection;
};
