#include <stdafx.h>

#include <GameServer.h>

#include <Scripting/Actor.h>

#include <World.h>
#include <Components.h>

#include <Messages/NotifyDeathStateChange.h>

namespace Script
{
static Inventory kNullInventory{};
static glm::vec3 kNullVec3{};

Actor::Actor(entt::entity aEntity, World& aWorld)
    : EntityHandle(aEntity, aWorld)
{
}

const glm::vec3& Actor::GetPosition() const
{
    if (auto movementComponent = m_pWorld->try_get<MovementComponent>(m_entity))
        return movementComponent->Position;
    return kNullVec3;
}

const glm::vec3& Actor::GetRotation() const
{
    if (auto movementComponent = m_pWorld->try_get<MovementComponent>(m_entity))
        return movementComponent->Rotation;
    return kNullVec3;
}

float Actor::GetSpeed() const
{
    auto movementComponent = m_pWorld->try_get<MovementComponent>(m_entity);

    return 0.f;
}

bool Actor::IsDead() const
{
    if (auto characterComponent = m_pWorld->try_get<CharacterComponent>(m_entity))
        return characterComponent->IsDead();
    return false;
}

const Inventory& Actor::GetInventory() const
{
    if (const auto* inventoryComponent = m_pWorld->try_get<InventoryComponent>(m_entity))
        return inventoryComponent->Content;
    return kNullInventory;
}

void Actor::Kill() const
{
    ChangeDeathState(true);
}

void Actor::Resurrect() const
{
    ChangeDeathState(false);
}

void Actor::ChangeDeathState(bool aIsDead) const
{
    auto characterComponent = m_pWorld->try_get<CharacterComponent>(m_entity);
    if (!characterComponent)
        return;

    bool isDead = characterComponent->IsDead();
    if (isDead == aIsDead)
        return;

    characterComponent->SetDead(aIsDead);

    NotifyDeathStateChange notify{};
    notify.Id = World::ToInteger(m_entity);
    notify.IsDead = aIsDead;

    GameServer::Get()->SendToPlayersInRange(notify, m_entity);
}
} // namespace Script
