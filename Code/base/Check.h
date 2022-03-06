// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

#include <spdlog/spdlog.h>

namespace base
{
static void Assert(const char* apExpression, const char* apMessage)
{
    spdlog::critical("Assertion failed ({}): {}", apExpression, apMessage);

#if defined(_WIN32)
    if (IsDebuggerPresent())
        __debugbreak();
#endif
}
} // namespace base

#define BASE_ASSERT(Expr, Msg, ...)                                                                                    \
    if (!(Expr))                                                                                                       \
    {                                                                                                                  \
        ::base::Assert(#Expr, fmt::format(Msg, ## __VA_ARGS__).c_str());                                               \
    }
