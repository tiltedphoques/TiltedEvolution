#include <Scripts/Player.h>
#include <Scripts/Npc.h>

#include <World.h>
#include <Components.h>

namespace Script
{
    Player::Player(entt::entity aEntity, World& aWorld)
        : EntityHandle(aEntity, aWorld)
    {}

    const Vector<String>& Player::GetMods() const
    {
        auto& playerComponent = m_pWorld->get<PlayerComponent>(m_entity);

        return playerComponent.Mods;
    }

    const String& Player::GetIp() const
    {
        auto& playerComponent = m_pWorld->get<PlayerComponent>(m_entity);

        return playerComponent.Endpoint;
    }

    const Vector3<float>& Player::GetPosition() const
    {
        auto& movementComponent = m_pWorld->get<MovementComponent>(m_entity);

        return movementComponent.Position;
    }

    const Vector3<float>& Player::GetRotation() const
    {
        auto& movementComponent = m_pWorld->get<MovementComponent>(m_entity);

        return movementComponent.Rotation;
    }

    float Player::GetSpeed() const
    {
        auto& movementComponent = m_pWorld->get<MovementComponent>(m_entity);

        return movementComponent.Variables.Floats[AnimationData::FloatVariables::kSpeed];
    }

    bool Player::AddComponent(sol::object aObject) const
    {
        auto& playerComponent = m_pWorld->get<PlayerComponent>(m_entity);

        // If the player has a character we add the component to the child, makes no sense to add to a player
        if(playerComponent.Character)
        {
            const Script::Npc npc(*playerComponent.Character, *m_pWorld);
            return npc.AddComponent(aObject);
        }

        return false;
    }
}
