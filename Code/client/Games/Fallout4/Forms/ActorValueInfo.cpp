#include <TiltedOnlinePCH.h>

#include <Forms/ActorValueInfo.h>

ActorValueInfo* ActorValueInfo::Resolve(uint32_t aId) noexcept
{
    using TGetActorValueInfoArray = ActorValueInfo**();
    POINTER_FALLOUT4(TGetActorValueInfoArray, GetActorValueInfoArray, 0x14006B1F0 - 0x140000000);

    return GetActorValueInfoArray()[aId];
}
