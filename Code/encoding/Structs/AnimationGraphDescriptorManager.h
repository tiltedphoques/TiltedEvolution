#pragma once

#include <Structs/AnimationGraphDescriptor.h>

using TiltedPhoques::Map;

struct AnimationGraphDescriptorManager
{
    TP_NOCOPYMOVE(AnimationGraphDescriptorManager);

    static AnimationGraphDescriptorManager& Get() noexcept;
    const AnimationGraphDescriptor* GetDescriptor(const char* acpName) const noexcept;

    struct Builder
    {
        Builder(AnimationGraphDescriptorManager& aManager, const char* acpName,
                AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept;
    };

protected:

    void Register(const char* acpName, AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept;

private:

    AnimationGraphDescriptorManager() noexcept;

    Map<TiltedPhoques::String, AnimationGraphDescriptor> m_descriptors;
};
