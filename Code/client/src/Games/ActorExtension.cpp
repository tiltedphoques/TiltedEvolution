#include <Games/ActorExtension.h>

void ActorExtension::InitializeExtension() noexcept
{
    onlineFlags = 0;
}

bool ActorExtension::IsRemote() const noexcept
{
    return onlineFlags & kRemote;
}

void ActorExtension::SetRemote(bool aSet) noexcept
{
    if (aSet)
        onlineFlags |= kRemote;
    else
        onlineFlags &= ~kRemote;
}
