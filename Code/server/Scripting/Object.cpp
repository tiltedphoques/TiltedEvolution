#include <Scripting/Object.h>

#include <GameServer.h>

#include <Events/ObjectLockEvent.h>

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
        if (objectComponent->CurrentLockData)
            return *objectComponent->CurrentLockData;
    }

    return kNullLockData;
}

void Object::Lock(uint8_t aLockLevel)
{
    LockData lock{};
    lock.IsLocked = true;
    lock.LockLevel = aLockLevel;
    m_pWorld->GetDispatcher().trigger(ObjectLockEvent(m_entity, lock));
}

void Object::Unlock()
{
    auto objectComponent = m_pWorld->try_get<ObjectComponent>(m_entity);
    if (!objectComponent || !objectComponent->CurrentLockData)
        return;

    LockData lock{};
    lock.IsLocked = false;
    lock.LockLevel = objectComponent->CurrentLockData->LockLevel;
    m_pWorld->GetDispatcher().trigger(ObjectLockEvent(m_entity, lock));
}

}
