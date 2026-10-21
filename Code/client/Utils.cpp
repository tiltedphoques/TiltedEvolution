#include <World.h>
#include <Components.h>
#include <Actor.h>

namespace Utils
{

std::optional<uint32_t> GetServerId(entt::entity aEntity) noexcept
{
    const auto* pLocalComponent = World::Get().try_get<LocalComponent>(aEntity);
    const auto* pRemoteComponent = World::Get().try_get<RemoteComponent>(aEntity);
    const auto* pObjectComponent = World::Get().try_get<ObjectComponent>(aEntity);

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
        spdlog::debug("{}: This entity has neither a local or remote component: {:X}, form id: {:X}", __FUNCTION__, to_integral(aEntity), pFormIdComponent ? pFormIdComponent->Id : 0);
        return std::nullopt;
    }

    return {serverId};
}

std::optional<entt::entity> FindEntityByServerId(const uint32_t aServerId) noexcept
{
    const auto localView = World::Get().view<LocalComponent>();
    const auto localIt = std::find_if(
        localView.begin(), localView.end(),
        [localView, aServerId](const entt::entity aEntity) { return localView.get<LocalComponent>(aEntity).Id == aServerId; });
    if (localIt != localView.end())
        return *localIt;

    const auto remoteView = World::Get().view<RemoteComponent>();
    const auto remoteIt = std::find_if(
        remoteView.begin(), remoteView.end(),
        [remoteView, aServerId](const entt::entity aEntity) { return remoteView.get<RemoteComponent>(aEntity).Id == aServerId; });
    if (remoteIt != remoteView.end())
        return *remoteIt;

    const auto objectView = World::Get().view<ObjectComponent>();
    const auto objectIt = std::find_if(
        objectView.begin(), objectView.end(),
        [objectView, aServerId](const entt::entity aEntity) { return objectView.get<ObjectComponent>(aEntity).Id == aServerId; });
    if (objectIt != objectView.end())
        return *objectIt;

    return std::nullopt;
}

std::optional<entt::entity> FindRemoteEntityAtEpoch(const uint32_t aServerId, const uint32_t aOwnershipEpoch) noexcept
{
    if (aOwnershipEpoch == 0)
        return std::nullopt;

    auto view = World::Get().view<RemoteComponent>(entt::exclude<LocalComponent>);
    const auto it = std::find_if(view.begin(), view.end(), [view, aServerId](const entt::entity aEntity) { return view.get<RemoteComponent>(aEntity).Id == aServerId; });
    if (it == view.end())
        return std::nullopt;

    auto& remoteComponent = view.get<RemoteComponent>(*it);
    if (aOwnershipEpoch < remoteComponent.OwnershipEpoch)
        return std::nullopt;

    if (aOwnershipEpoch > remoteComponent.OwnershipEpoch)
    {
        spdlog::info("Adopting ownership epoch {} for remote actor {:X}, the transfer from epoch {} was missed", aOwnershipEpoch, aServerId, remoteComponent.OwnershipEpoch);
        remoteComponent.OwnershipEpoch = aOwnershipEpoch;
    }

    return *it;
}

void ShowHudMessage(const TiltedPhoques::String& acMessage)
{
    using TShowHudMessage = void(const char*, const char*, bool);

    POINTER_SKYRIMSE(TShowHudMessage, s_showHudMessage, 52933);

    s_showHudMessage(acMessage.c_str(), nullptr, false);
}

} // namespace Utils
