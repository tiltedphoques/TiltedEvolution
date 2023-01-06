#include <stdafx.h>

#include <GameServer.h>

#include <Scripting/Npc.h>

#include <World.h>
#include <Components.h>

#include <Messages/NotifyDeathStateChange.h>

namespace Script
{
Npc::Npc(entt::entity aEntity, World& aWorld)
    : EntityHandle(aEntity, aWorld)
{
}

const glm::vec3& Npc::GetPosition() const
{
    auto& movementComponent = m_pWorld->get<MovementComponent>(m_entity);

    return movementComponent.Position;
}

const glm::vec3& Npc::GetRotation() const
{
    auto& movementComponent = m_pWorld->get<MovementComponent>(m_entity);

    return movementComponent.Rotation;
}

float Npc::GetSpeed() const
{
    auto& movementComponent = m_pWorld->get<MovementComponent>(m_entity);

    return 0.f;
}

bool Npc::IsDead() const
{
    auto& characterComponent = m_pWorld->get<CharacterComponent>(m_entity);

    return characterComponent.IsDead();
}

void Npc::Kill() const
{
    auto& characterComponent = m_pWorld->get<CharacterComponent>(m_entity);
    if (characterComponent.IsDead())
        return;

    characterComponent.SetDead(true);

    NotifyDeathStateChange notify{};
    notify.Id = World::ToInteger(m_entity);
    notify.IsDead = true;

    GameServer::Get()->SendToPlayersInRange(notify, m_entity);
}

void Npc::Resurrect() const
{
    auto& characterComponent = m_pWorld->get<CharacterComponent>(m_entity);
    if (!characterComponent.IsDead())
        return;

    characterComponent.SetDead(false);

    NotifyDeathStateChange notify{};
    notify.Id = World::ToInteger(m_entity);
    notify.IsDead = false;

    GameServer::Get()->SendToPlayersInRange(notify, m_entity);
}
} // namespace Script
