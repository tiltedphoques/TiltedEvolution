#include <stdafx.h>

#include <GameServer.h>
#include <World.h>

#include <Components.h>
#include <Events/PlayerJoinEvent.h>
#include <Events/PlayerLeaveCellEvent.h>
#include <Services/SkyService.h>
#include <Messages/ServerSkySeed.h>

#include <base/Random.h>

const Chunks::RDWT* GetRandomWeather(World &aWorld)
{
    uint32_t cumulatedChance = 0;

    const auto pRegion = aWorld.GetRecordCollection()->GetRegions().begin().value();

    for (auto& regionNode : pRegion.m_weatherTypes)
    {
        float fValue;
        if (regionNode.m_globalId)
            assert(true);
            //cumulatedChance += pChanceVar->fValue;
        else
            cumulatedChance += regionNode.m_chance;
    }

    if (!cumulatedChance)
        return nullptr;

    uint32_t u8 = -1;
    uint32_t v1 = 0;
    auto random = base::RandomIntegral<uint32_t>(0, UINT_MAX) % cumulatedChance;
    for (auto& regionNode : pRegion.m_weatherTypes)
    {
        float fValue;
        if (regionNode.m_globalId)
            assert(true);
        // cumulatedChance += pChanceVar->fValue;
        else
            cumulatedChance += regionNode.m_chance;

        uint32_t chance = regionNode.m_globalId ? 1337 : regionNode.m_chance;
        u8 += chance;

        if (v1 <= random && random <= u8)
        {
            return &regionNode;
        }

        v1 += chance;
    }

    return nullptr;
}

SkyService::SkyService(World& aWorld, entt::dispatcher& aDispatcher)
    : m_world(aWorld), m_envService(aWorld.ctx<EnvironmentService>()), m_nextUpdate(std::chrono::seconds(0))
{
    // initial state.
    m_weatherSeed = base::RandomIntegral<uint32_t>(0, UINT_MAX);

    m_updateConnection = aDispatcher.sink<UpdateEvent>().connect<&SkyService::OnUpdate>(this);
    m_joinConnection = aDispatcher.sink<PlayerJoinEvent>().connect<&SkyService::OnPlayerJoin>(this);

    for (auto& climate : m_world.GetRecordCollection()->GetClimates())
    {
        spdlog::info("[SkyService]: Loading Climate: {}", climate.second.m_editorId.c_str());
    }

    for (auto& region : m_world.GetRecordCollection()->GetRegions())
    {
        spdlog::info("[SkyService]: Loading Region: {}", region.second.m_editorId.c_str());
    }

    /*
    struct FORM
{
  unsigned int form;
  unsigned int length;
  unsigned int flags;
  unsigned int iFormID;
  unsigned int iVersionControl;
  unsigned __int16 sFormVersion;
  unsigned __int16 sVCVersion;
};
*/

   // auto randomWeather = GetRandomWeather(aWorld);

    // weatherlist component.
}

void SkyService::OnPlayerJoin(const PlayerJoinEvent& acEvent) const noexcept
{
    // TODO: send previous seed, so we can do corrections later on.

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

        //constexpr float xx = 255u* 2.3283064e-10f;

        // todo: calculate the delay based on the server.
        m_nextUpdate = (std::chrono::steady_clock::now() + std::chrono::seconds(20));
    }
}
