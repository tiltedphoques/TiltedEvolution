#include <stdafx.h>

#include <GameServer.h>

#include <Scripting/Actor.h>

#include <World.h>
#include <Components.h>

#include <Messages/NotifyDeathStateChange.h>
#include <Messages/NotifyHealthChangeBroadcast.h>
#include <Messages/NotifyActorMaxValueChanges.h>

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

float Actor::GetHealth() const
{
    if (auto actorValuesComponent = m_pWorld->try_get<ActorValuesComponent>(m_entity))
        return actorValuesComponent->CurrentActorValues.ActorValuesList[24];
    return 0.f;
}

float Actor::GetHealthMax() const
{
    if (auto actorValuesComponent = m_pWorld->try_get<ActorValuesComponent>(m_entity))
        return actorValuesComponent->CurrentActorValues.ActorMaxValuesList[24];
    return 0.f;
}

void Actor::SetHealth(float aHealth)
{
    auto actorValuesComponent = m_pWorld->try_get<ActorValuesComponent>(m_entity);
    if (!actorValuesComponent)
        return;

    // Disallow setting the health value higher than the max
    if (aHealth > actorValuesComponent->CurrentActorValues.ActorMaxValuesList[24])
        return;

    float currentHealth = actorValuesComponent->CurrentActorValues.ActorValuesList[24];

    NotifyHealthChangeBroadcast notify{};
    notify.Id = World::ToInteger(m_entity);
    notify.DeltaHealth = aHealth - currentHealth;

    GameServer::Get()->SendToPlayers(notify);
}

void Actor::SetHealthMax(float aHealthMax)
{
    auto actorValuesComponent = m_pWorld->try_get<ActorValuesComponent>(m_entity);
    if (!actorValuesComponent)
        return;

    actorValuesComponent->CurrentActorValues.ActorMaxValuesList[24] = aHealthMax;

    NotifyActorMaxValueChanges notify{};
    notify.Id = World::ToInteger(m_entity);
    notify.Values = {{24, aHealthMax}};

    GameServer::Get()->SendToPlayers(notify);
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
