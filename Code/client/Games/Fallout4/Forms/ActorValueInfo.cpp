#include <TiltedOnlinePCH.h>

#include <Forms/ActorValueInfo.h>

ActorValueInfo* ActorValueInfo::Resolve(uint32_t aId) noexcept
{
    using TGetActorValueInfoArray = ActorValueInfo**();
    POINTER_FALLOUT4(TGetActorValueInfoArray, GetActorValueInfoArray, 405391);

    return GetActorValueInfoArray()[aId];
}
