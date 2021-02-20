#include <TiltedOnlinePCH.h>

#include <Systems/CacheSystem.h>
#include <World.h>

#include <Games/References.h>

// We don't clean up after ourselves since only a server removal should clean and server removals destroy entities entirely
void CacheSystem::Setup(World& aWorld, const entt::entity aEntity, Actor* apActor) noexcept
{
    auto& cacheComponent = aWorld.emplace_or_replace<CacheComponent>(aEntity);
    cacheComponent.FactionsContent = apActor->GetFactions();
}
