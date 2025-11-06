#pragma once

#include <string_view>

using TiltedPhoques::Set;

namespace AnimationEventLists
{
extern const Map<std::string_view, std::string_view> kIdleToInstant;

extern const Set<std::string_view> kExitSpecial;

extern const Set<std::string_view> kIgnore;
} // namespace
