#pragma once

#include <Scripting/EntityHandle.h>

namespace Script
{
struct Actor : EntityHandle
{
    Actor(entt::entity aEntity, World& aWorld);

    [[nodiscard]] const glm::vec3& GetPosition() const;
    [[nodiscard]] const glm::vec3& GetRotation() const;
    [[nodiscard]] float GetSpeed() const;
    [[nodiscard]] bool IsDead() const;

    const Inventory& GetInventory() const;

    void Kill() const;
    void Resurrect() const;

    Actor& operator=(const Actor& acRhs)
    {
        EntityHandle::operator=(acRhs);

        return *this;
    }

protected:
    void ChangeDeathState(bool aIsDead) const;
};
} // namespace Script
