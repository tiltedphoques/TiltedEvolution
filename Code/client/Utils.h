#pragma once

#if defined(TP_SKYRIM) && TP_PLATFORM_64
#define POINTER_SKYRIMSE(className, variableName, ...) static AutoPtr<className> variableName(__VA_ARGS__)
#else
#define POINTER_SKYRIMSE(className, variableName, ...) ;
#endif

#if defined(TP_FALLOUT) && TP_PLATFORM_64
#define POINTER_FALLOUT4(className, variableName, ...) static AutoPtr<className> variableName(__VA_ARGS__)
#else
#define POINTER_FALLOUT4(className, variableName, ...) ;
#endif

// TODO: should this be debug only? I removed the check since debug is broken, can only use releasedbg
#define TP_ASSERT(Expr, Msg, ...) \
    if (!(Expr)) \
    { \
        utils::Assert(#Expr, fmt::format(Msg, __VA_ARGS__).c_str()); \
    }

namespace utils
{
static void Assert(const char* apExpression, const char* apMessage)
{
    spdlog::critical("Assertion failed ({}): {}", apExpression, apMessage);

    if (IsDebuggerPresent())
        __debugbreak();
}
}

