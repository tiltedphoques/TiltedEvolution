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
        spdlog::warn("This entity has neither a local or remote component: {:X}, form id: {:X}",  aEntity, pFormIdComponent ? pFormIdComponent->Id : 0);
        return std::nullopt;
    }

    return {serverId};
}

std::optional<Actor*> GetActorByServerId(const uint32_t aServerId) noexcept
{
    auto view = World::Get().view<FormIdComponent>();

    for (entt::entity entity : view)
    {
        std::optional<uint32_t> serverIdRes = Utils::GetServerId(entity);
        if (!serverIdRes.has_value())
            continue;

        uint32_t serverId = serverIdRes.value();

        if (serverId == aServerId)
        {
            const auto& formIdComponent = view.get<FormIdComponent>(entity);
            const TESForm* pForm = TESForm::GetById(formIdComponent.Id);
            Actor* pActor = RTTI_CAST(pForm, TESForm, Actor);

            if (pActor != nullptr)
            {
                return {pActor};
            }
        }
    }

    spdlog::warn("Actor not found for server id {:X}", aServerId);
    return std::nullopt;
}
}
