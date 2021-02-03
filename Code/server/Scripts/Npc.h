#pragma once

#include <Scripts/EntityHandle.h>

namespace Script
{
    struct Npc : EntityHandle
    {
        Npc(entt::entity aEntity, World& aWorld);

        [[nodiscard]] const Vector3<float>& GetPosition() const;
        [[nodiscard]] const Vector3<float>& GetRotation() const;
        [[nodiscard]] float GetSpeed() const;

        Npc& operator=(const Npc& acRhs)
        {
            EntityHandle::operator=(acRhs);

            return *this;
        }
    };
}
