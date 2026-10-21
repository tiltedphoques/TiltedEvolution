#pragma once

#include <optional>
#include "VersionDb.h"
#include "World.h"

#define POINTER_SKYRIMSE(className, variableName, ...) static VersionDbPtr<className> variableName(__VA_ARGS__)
#define POINTER_SKYRIMSE_LEGACY(className, variableName, ...) static AutoPtr<decltype()> variableName(__VA_ARGS__)

// TODO: should this be debug only? I removed the check since debug is broken, can only use releasedbg
#define TP_ASSERT(Expr, Msg, ...)                                    \
    if (!(Expr))                                                     \
    {                                                                \
        Utils::Assert(#Expr, fmt::format(Msg, __VA_ARGS__).c_str()); \
    }

struct TESForm;

namespace Utils
{

static void Assert(const char* apExpression, const char* apMessage)
{
    spdlog::critical("Assertion failed ({}): {}", apExpression, apMessage);

    if (IsDebuggerPresent())
        __debugbreak();
}

std::optional<uint32_t> GetServerId(entt::entity aEntity) noexcept;
std::optional<entt::entity> FindEntityByServerId(uint32_t aServerId) noexcept;
// Finds the remote entity a relayed owner update belongs to. The server only relays the current owner at its current
// epoch, so a newer epoch means this client missed the ownership transfer; it is adopted instead of dropping every later update.
std::optional<entt::entity> FindRemoteEntityAtEpoch(uint32_t aServerId, uint32_t aOwnershipEpoch) noexcept;

template <class T> T* GetByServerId(const uint32_t acServerId) noexcept
{
    if (const auto entity = FindEntityByServerId(acServerId))
    {
        if (const auto* pFormIdComponent = World::Get().try_get<FormIdComponent>(*entity))
        {
            TESForm* pForm = TESForm::GetById(pFormIdComponent->Id);

            if (pForm)
                return Cast<T>(pForm);
        }
    }

    spdlog::debug("{}: form not found for server id {:X}", __FUNCTION__, acServerId);
    return nullptr;
}

void ShowHudMessage(const TiltedPhoques::String& acMessage);
} // namespace Utils

namespace TiltedPhoques
{
template <class TFunc, class TThis, class... TArgs> constexpr decltype(auto) ThisCall(TFunc* aFunction, VersionDbPtr<TThis>& aThis, TArgs&&... args) noexcept
{
    return ThisCall(aFunction, aThis.Get(), args...);
}

template <class TFunc, class TThis, class... TArgs> constexpr decltype(auto) ThisCall(VersionDbPtr<TFunc>& aFunction, VersionDbPtr<TThis>& aThis, TArgs&&... args) noexcept
{
    return ThisCall(aFunction.Get(), aThis.Get(), args...);
}

template <class TFunc, class TThis, class... TArgs> constexpr decltype(auto) ThisCall(VersionDbPtr<TFunc>& aFunction, TThis* apThis, TArgs&&... args) noexcept
{
    return ThisCall(aFunction.Get(), apThis, args...);
}
} // namespace TiltedPhoques
