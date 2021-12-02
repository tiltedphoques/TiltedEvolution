#include <World.h>
#include <Components.h>

namespace utils
{
std::optional<uint32_t> GetServerId(entt::entity aEntity) noexcept
{
    const auto* pLocalComponent = World::Get().try_get<LocalComponent>(aEntity);
    const auto* pRemoteComponent = World::Get().try_get<RemoteComponent>(aEntity);

    uint32_t serverId = -1;
    if (pLocalComponent)
        serverId = pLocalComponent->Id;
    else if (pRemoteComponent)
        serverId = pRemoteComponent->Id;
    else
    {
        spdlog::warn("This entity has neither a local or remote component: {:X}",  aEntity);
        return std::nullopt;
    }

    return {serverId};
}
}
