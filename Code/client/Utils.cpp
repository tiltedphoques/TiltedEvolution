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
        spdlog::warn("This entity has neither a local or remote component: {:X}, form id: {:X}",  to_integral(aEntity), pFormIdComponent ? pFormIdComponent->Id : 0);
        return std::nullopt;
    }

    return {serverId};
}

void ShowHudMessage(const TiltedPhoques::String& acMessage)
{
#if TP_SKYRIM64
    using TShowHudMessage = void(const char*, const char*, bool);
#elif TP_FALLOUT4
    using TShowHudMessage = void(const char*, const char*, bool, bool);
#endif

    POINTER_SKYRIMSE(TShowHudMessage, s_showHudMessage, 52933);
    POINTER_FALLOUT4(TShowHudMessage, s_showHudMessage, 1163006);

#if TP_SKYRIM64
    s_showHudMessage(acMessage.c_str(), nullptr, false);
#elif TP_FALLOUT4
    s_showHudMessage(acMessage.c_str(), nullptr, false, false);
#endif
}

} // namespace Utils
