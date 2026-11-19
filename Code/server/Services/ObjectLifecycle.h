#pragma once

#include <entt/entity/registry.hpp>
#include <functional>
#include <span>

struct GameId;
struct NotifyRemoveObjects;

namespace ObjectLifecycle
{
using SendRemoval = std::function<void(const NotifyRemoveObjects&)>;

// aPlayerCells contains the players' current cells after the departure.
// aSendRemoval must broadcast synchronously before entity identifiers can be reused.
void RetireCell(entt::registry& aRegistry, const GameId& acCell, std::span<const GameId> aPlayerCells, const SendRemoval& aSendRemoval) noexcept;
} // namespace ObjectLifecycle
