#pragma once

#include <Scripts/EntityHandle.h>

namespace Script
{
    struct Player : EntityHandle
    {
        Player(entt::entity aEntity, World& aWorld);

        const Vector<String>& GetMods() const;
        const String& GetIp() const;

        [[nodiscard]] const Vector3<float>& GetPosition() const;
        [[nodiscard]] const Vector3<float>& GetRotation() const;
        [[nodiscard]] float GetSpeed() const;

        Player& operator=(const Player& acRhs)
        {
            EntityHandle::operator=(acRhs);

            return *this;
        }

        bool AddComponent(sol::object aObject) const override;
    };
}
