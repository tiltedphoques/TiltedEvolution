#pragma once

#include <Scripting/EntityHandle.h>

namespace Script
{
struct Npc : EntityHandle
{
    Npc(entt::entity aEntity, World& aWorld);

    [[nodiscard]] const glm::vec3& GetPosition() const;
    [[nodiscard]] const glm::vec3& GetRotation() const;
    [[nodiscard]] float GetSpeed() const;

    Npc& operator=(const Npc& acRhs)
    {
        EntityHandle::operator=(acRhs);

        return *this;
    }
};
} // namespace Script
