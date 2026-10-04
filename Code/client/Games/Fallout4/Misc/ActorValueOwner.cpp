#include <TiltedOnlinePCH.h>

#include <Misc/ActorValueOwner.h>
#include <Forms/ActorValueInfo.h>

float ActorValueOwner::GetValue(uint32_t aId) const noexcept
{
    const auto* pInfo = ActorValueInfo::Resolve(aId);
    return pInfo ? GetNativeValue(*pInfo) : 0.f;
}

float ActorValueOwner::GetPermanentValue(uint32_t aId) const noexcept
{
    const auto* pInfo = ActorValueInfo::Resolve(aId);
    return pInfo ? GetNativePermanentValue(*pInfo) : 0.f;
}

float ActorValueOwner::GetBaseValue(uint32_t aId) const noexcept
{
    const auto* pInfo = ActorValueInfo::Resolve(aId);
    return pInfo ? GetNativeBaseValue(*pInfo) : 0.f;
}

void ActorValueOwner::SetBaseValue(uint32_t aId, float aValue)
{
    if (const auto* pInfo = ActorValueInfo::Resolve(aId))
        SetNativeBaseValue(*pInfo, aValue);
}

void ActorValueOwner::ModValue(uint32_t aId, float aValue)
{
    if (const auto* pInfo = ActorValueInfo::Resolve(aId))
        ModNativeBaseValue(*pInfo, aValue);
}

void ActorValueOwner::ForceCurrent(ForceMode aMode, uint32_t aId, float aValue)
{
    if (const auto* pInfo = ActorValueInfo::Resolve(aId))
        ModNativeValue(aMode, *pInfo, aValue);
}

void ActorValueOwner::SetValue(uint32_t aId, float aValue) noexcept
{
    if (const auto* pInfo = ActorValueInfo::Resolve(aId))
        SetNativeValue(*pInfo, aValue);
}
