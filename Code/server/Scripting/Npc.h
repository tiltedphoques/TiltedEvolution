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
    [[nodiscard]] bool IsDead() const;

    void Kill() const;
    void Resurrect() const;

    Npc& operator=(const Npc& acRhs)
    {
        EntityHandle::operator=(acRhs);

        return *this;
    }

protected:
    void ChangeDeathState(bool aIsDead) const;
};
} // namespace Script
