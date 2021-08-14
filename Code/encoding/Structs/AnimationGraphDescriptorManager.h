#pragma once

#include <Structs/AnimationGraphDescriptor.h>

using TiltedPhoques::Map;

struct AnimationGraphDescriptorManager
{
    TP_NOCOPYMOVE(AnimationGraphDescriptorManager);

    static AnimationGraphDescriptorManager& Get() noexcept;
    const AnimationGraphDescriptor* GetDescriptor(uint64_t aKey) const noexcept;

    struct Builder
    {
        Builder(AnimationGraphDescriptorManager& aManager, uint64_t aKey,
                AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept;
    };

protected:

    void Register(uint64_t aKey, AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept;

private:

    AnimationGraphDescriptorManager() noexcept;

    Map<uint64_t, AnimationGraphDescriptor> m_descriptors;
};
