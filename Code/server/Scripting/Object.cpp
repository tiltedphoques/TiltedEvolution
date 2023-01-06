#include <Scripting/Object.h>

#include <GameServer.h>

#include <Messages/NotifyLockChange.h>

namespace Script
{
static LockData kNullLockData{};

Object::Object(entt::entity aEntity, World& aWorld)
    : EntityHandle(aEntity, aWorld)
{
}

const LockData& Object::GetLockData()
{
    if (auto objectComponent = m_pWorld->try_get<ObjectComponent>(m_entity))
    {
        return objectComponent->CurrentLockData;
    }

    return kNullLockData;
}

void Object::Lock(uint8_t aLockLevel)
{
    auto objectComponent = m_pWorld->try_get<ObjectComponent>(m_entity);
    auto formIdComponent = m_pWorld->try_get<FormIdComponent>(m_entity);

    if (!objectComponent || !formIdComponent)
        return;

    objectComponent->CurrentLockData.IsLocked = true;
    objectComponent->CurrentLockData.LockLevel = aLockLevel;

    NotifyLockChange notifyLockChange{};
    notifyLockChange.Id = formIdComponent->Id;
    notifyLockChange.IsLocked = true;
    notifyLockChange.LockLevel = aLockLevel;

    GameServer::Get()->SendToPlayersInRange(notifyLockChange, m_entity);
}

void Object::Unlock()
{
    auto objectComponent = m_pWorld->try_get<ObjectComponent>(m_entity);
    auto formIdComponent = m_pWorld->try_get<FormIdComponent>(m_entity);

    if (!objectComponent || !formIdComponent)
        return;

    objectComponent->CurrentLockData.IsLocked = false;

    NotifyLockChange notifyLockChange{};
    notifyLockChange.Id = formIdComponent->Id;
    notifyLockChange.IsLocked = false;

    GameServer::Get()->SendToPlayersInRange(notifyLockChange, m_entity);
}

}
