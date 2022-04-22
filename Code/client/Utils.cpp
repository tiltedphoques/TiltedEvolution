#include <World.h>
#include <Components.h>
#include <Actor.h>

namespace Utils
{

std::optional<uint32_t> GetServerId(entt::entity aEntity) noexcept
{
    const auto* pLocalComponent = World::Get().try_get<LocalComponent>(aEntity);
    const auto* pRemoteComponent = World::Get().try_get<RemoteComponent>(aEntity);
    const auto* pObjectComponent = World::Get().try_get<InteractiveObjectComponent>(aEntity);

    uint32_t serverId = -1;
    if (pLocalComponent)
        serverId = pLocalComponent->Id;
    else if (pRemoteComponent)
        serverId = pRemoteComponent->Id;
    else if (pObjectComponent)
        serverId = pObjectComponent->Id;
    else
    {
        const auto* pFormIdComponent = World::Get().try_get<FormIdComponent>(aEntity);
        spdlog::warn("This entity has neither a local or remote component: {:X}, form id: {:X}",  to_integral(aEntity), pFormIdComponent ? pFormIdComponent->Id : 0);
        return std::nullopt;
    }

    return {serverId};
}

} // namespace Utils
