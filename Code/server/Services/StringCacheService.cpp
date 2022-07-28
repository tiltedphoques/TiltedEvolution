#include "StringCache.h"

#include <GameServer.h>
#include <Services/StringCacheService.h>
#include <Events/UpdateEvent.h>
#include <Game/Player.h>

StringCacheService::StringCacheService(World& aWorld, entt::dispatcher& aDispatcher)
    : m_world(aWorld),
      m_updateConnection(aDispatcher.sink<UpdateEvent>().connect<&StringCacheService::HandleUpdate>(this))
{
}

void StringCacheService::HandleUpdate(const UpdateEvent&) const noexcept
{
    static std::chrono::steady_clock::time_point lastSendTimePoint;
    constexpr auto cDelayBetweenSnapshots = 2000ms;

    const auto now = std::chrono::steady_clock::now();
    if (now - lastSendTimePoint < cDelayBetweenSnapshots)
        return;

    lastSendTimePoint = now;

    auto& stringCache = StringCache::Get();

    if (!stringCache.ProcessDirty())
        return;

    for (const auto pPlayer : m_world.GetPlayerManager())
    {
        auto startId = pPlayer->GetStringCacheId();
        auto update = stringCache.Serialize(startId);
        pPlayer->SetStringCacheId(startId);

        pPlayer->Send(update);
    }
}
