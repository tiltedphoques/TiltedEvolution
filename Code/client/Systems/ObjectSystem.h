#pragma once

#include <cstdint>
#include <span>
#include <entt/fwd.hpp>

/**
 * @brief Manages object replication bindings without changing game references.
 */
struct ObjectSystem
{
    static entt::entity Setup(entt::registry& aRegistry, uint32_t aFormId, uint32_t aServerId) noexcept;
    static void Remove(entt::registry& aRegistry, std::span<const uint32_t> aServerIds) noexcept;
    static void Clear(entt::registry& aRegistry) noexcept;
};
