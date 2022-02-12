#pragma once

#include <optional>
#include "VersionDb.h"

#if defined(TP_SKYRIM) && TP_PLATFORM_64
#define POINTER_SKYRIMSE(className, variableName, ...) static VersionDbPtr<className> variableName(__VA_ARGS__)
//#define POINTER_SKYRIMSE_V2(variableName, ...) static AutoPtr<decltype()> variableName(__VA_ARGS__)
#else
#define POINTER_SKYRIMSE(className, variableName, ...) ;
#endif

#if defined(TP_FALLOUT) && TP_PLATFORM_64
#define POINTER_FALLOUT4(className, variableName, ...) static AutoPtr<className> variableName(__VA_ARGS__)
#else
#define POINTER_FALLOUT4(className, variableName, ...) ;
#endif

// TODO: should this be debug only? I removed the check since debug is broken, can only use releasedbg
#define TP_ASSERT(Expr, Msg, ...)                                                                                      \
    if (!(Expr))                                                                                                       \
    {                                                                                                                  \
        Utils::Assert(#Expr, fmt::format(Msg, __VA_ARGS__).c_str());                                                   \
    }

namespace Utils
{
static void Assert(const char* apExpression, const char* apMessage)
{
    spdlog::critical("Assertion failed ({}): {}", apExpression, apMessage);

    if (IsDebuggerPresent())
        __debugbreak();
}

std::optional<uint32_t> GetServerId(entt::entity aEntity) noexcept;
} // namespace Utils

namespace TiltedPhoques
{
template <class TFunc, class TThis, class... TArgs>
constexpr decltype(auto) ThisCall(TFunc* aFunction, VersionDbPtr<TThis>& aThis, TArgs&&... args) noexcept
{
    return ThisCall(aFunction, aThis.Get(), args...);
}

template <class TFunc, class TThis, class... TArgs>
constexpr decltype(auto) ThisCall(VersionDbPtr<TFunc>& aFunction, VersionDbPtr<TThis>& aThis, TArgs&&... args) noexcept
{
    return ThisCall(aFunction.Get(), aThis.Get(), args...);
}

template <class TFunc, class TThis, class... TArgs>
constexpr decltype(auto) ThisCall(VersionDbPtr<TFunc>& aFunction, TThis* apThis, TArgs&&... args) noexcept
{
    return ThisCall(aFunction.Get(), apThis, args...);
}
} // namespace TiltedPhoques
