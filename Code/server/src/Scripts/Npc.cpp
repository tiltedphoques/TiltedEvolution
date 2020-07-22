#include <Scripts/Npc.h>

#include <World.h>
#include <Components.h>

namespace Script
{
    Npc::Npc(entt::entity aEntity, World& aWorld)
        : EntityHandle(aEntity, aWorld)
    {}

    const Vector3<float>& Npc::GetPosition() const
    {
        auto& movementComponent = m_pWorld->get<MovementComponent>(m_entity);

        return movementComponent.Position;
    }

    const Vector3<float>& Npc::GetRotation() const
    {
        auto& movementComponent = m_pWorld->get<MovementComponent>(m_entity);

        return movementComponent.Rotation;
    }

    float Npc::GetSpeed() const
    {
        auto& movementComponent = m_pWorld->get<MovementComponent>(m_entity);

        return movementComponent.Variables.Floats[AnimationData::FloatVariables::kSpeed];
    }
}
