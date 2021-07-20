#pragma once

#include <Structs/AnimationGraphDescriptor.h>

using TiltedPhoques::Map;

struct AnimationGraphDescriptorManager
{
    TP_NOCOPYMOVE(AnimationGraphDescriptorManager);

    static AnimationGraphDescriptorManager& Get() noexcept;
    const AnimationGraphDescriptor* GetDescriptor(size_t aKey) const noexcept;

    struct Builder
    {
        Builder(AnimationGraphDescriptorManager& aManager, size_t aKey,
                AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept;
    };

protected:

    void Register(size_t aKey, AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept;

private:

    AnimationGraphDescriptorManager() noexcept;

    Map<size_t, AnimationGraphDescriptor> m_descriptors;
};
