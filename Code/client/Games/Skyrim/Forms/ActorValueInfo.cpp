#include <TiltedOnlinePCH.h>

#include <Forms/ActorValueInfo.h>

uint32_t ActorValueInfo::Resolve(uint32_t aId) noexcept
{
    return aId; // no-op this is just used in fallout4 but for ease of use we use this function to avoid game specific code
}
