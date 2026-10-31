#include <TiltedOnlinePCH.h>

#include <Systems/OwnershipSystem.h>
#include <World.h>

std::optional<entt::entity> OwnershipSystem::AcceptRemoteUpdate(World& aWorld, const uint32_t aServerId, const uint32_t aOwnershipEpoch) noexcept
{
    if (aOwnershipEpoch == 0)
        return std::nullopt;

    auto view = aWorld.view<RemoteComponent>(entt::exclude<LocalComponent>);
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

std::optional<entt::entity> OwnershipSystem::AcceptOwnerUpdate(World& aWorld, const uint32_t aServerId, const uint32_t aOwnershipEpoch) noexcept
{
    if (aOwnershipEpoch == 0)
        return std::nullopt;

    auto view = aWorld.view<LocalComponent>();
    const auto it = std::find_if(view.begin(), view.end(), [view, aServerId, aOwnershipEpoch](const entt::entity aEntity)
    {
        const auto& localComponent = view.get<LocalComponent>(aEntity);
        return localComponent.Id == aServerId && localComponent.OwnershipEpoch == aOwnershipEpoch;
    });

    if (it == view.end())
        return std::nullopt;

    return *it;
}

std::optional<OwnershipSystem::Ownership> OwnershipSystem::GetOutgoingOwnership(World& aWorld, const entt::entity aEntity, const bool aAllowRemote) noexcept
{
    if (const auto* pLocalComponent = aWorld.try_get<LocalComponent>(aEntity); pLocalComponent && pLocalComponent->OwnershipEpoch != 0)
        return Ownership{pLocalComponent->Id, pLocalComponent->OwnershipEpoch};

    if (!aAllowRemote)
        return std::nullopt;

    if (const auto* pRemoteComponent = aWorld.try_get<RemoteComponent>(aEntity); pRemoteComponent && pRemoteComponent->OwnershipEpoch != 0)
        return Ownership{pRemoteComponent->Id, pRemoteComponent->OwnershipEpoch};

    return std::nullopt;
}

uint32_t OwnershipSystem::GetOwnershipEpoch(World& aWorld, const entt::entity aEntity) noexcept
{
    if (const auto* pLocalComponent = aWorld.try_get<LocalComponent>(aEntity))
        return pLocalComponent->OwnershipEpoch;

    if (const auto* pRemoteComponent = aWorld.try_get<RemoteComponent>(aEntity))
        return pRemoteComponent->OwnershipEpoch;

    return 0;
}
