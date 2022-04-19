#include <World.h>
#include <Components.h>
#include <Actor.h>
#include <random>

namespace Utils
{

String RandomString(size_t aLength)
{
    constexpr char kAllowedCharacters[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device device;
    std::mt19937 generator(device());
    const std::uniform_int_distribution<> distribution(0, std::size(kAllowedCharacters) - 1);

    String result;

    for (std::size_t i = 0; i < aLength; ++i)
    {
        result += kAllowedCharacters[distribution(generator)];
    }

    return result;
}

TiltedPhoques::WString RandomStringW(size_t aLength)
{
    constexpr wchar_t kAllowedCharacters[] = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device device;
    std::mt19937 generator(device());
    const std::uniform_int_distribution<> distribution(0, std::size(kAllowedCharacters) - 1);

    TiltedPhoques::WString result;

    for (std::size_t i = 0; i < aLength; ++i)
    {
        result += kAllowedCharacters[distribution(generator)];
    }

    return result;
}

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

TESForm* GetFormByServerId(const uint32_t acServerId) noexcept
{
    auto view = World::Get().view<FormIdComponent>();

    for (entt::entity entity : view)
    {
        std::optional<uint32_t> serverIdRes = Utils::GetServerId(entity);
        if (!serverIdRes.has_value())
            continue;

        uint32_t serverId = serverIdRes.value();

        if (serverId == acServerId)
        {
            const auto& formIdComponent = view.get<FormIdComponent>(entity);
            TESForm* pForm = TESForm::GetById(formIdComponent.Id);

            if (pForm != nullptr)
            {
                return pForm;
            }
        }
    }

    spdlog::warn("Form not found for server id {:X}", acServerId);
    return nullptr;
}

} // namespace Utils
