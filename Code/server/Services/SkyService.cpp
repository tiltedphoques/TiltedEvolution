#include <stdafx.h>

#include <GameServer.h>
#include <World.h>

#include <Components.h>
#include <Events/PlayerJoinEvent.h>
#include <Events/PlayerLeaveCellEvent.h>
#include <Services/SkyService.h>
#include <Messages/ServerSkySeed.h>

#include <base/Random.h>

SkyService::SkyService(World& aWorld, entt::dispatcher& aDispatcher)
    : m_world(aWorld), m_envService(aWorld.ctx<EnvironmentService>()), m_nextUpdate(std::chrono::seconds(0))
{
    // initial state.
    m_weatherSeed = base::RandomIntegral<uint32_t>(0, UINT_MAX);

    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&SkyService::OnUpdate>(this);
    m_joinConnection = aDispatcher.sink<PlayerJoinEvent>().connect<&SkyService::OnPlayerJoin>(this);
}

void SkyService::OnPlayerJoin(const PlayerJoinEvent& acEvent) const noexcept
{
    ServerSkySeed seedMsg;
    seedMsg.Seed = m_weatherSeed;
    acEvent.pPlayer->Send(seedMsg);
}

void SkyService::OnUpdate(const UpdateEvent&) noexcept
{
    if (m_nextUpdate < std::chrono::steady_clock::now())
    {
        // refresh
        m_weatherSeed = base::RandomIntegral<uint32_t>(0, UINT_MAX);
        
        for (Player* pPlayer : m_world.GetPlayerManager())
        {
            ServerSkySeed seedMsg;
            seedMsg.Seed = m_weatherSeed;
            pPlayer->Send(seedMsg);
        }

        // todo: calculate the delay based on the server.
        m_nextUpdate = (std::chrono::steady_clock::now() + std::chrono::seconds(20));
    }
}
