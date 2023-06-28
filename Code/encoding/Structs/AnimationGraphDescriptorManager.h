#pragma once

#include <Structs/AnimationGraphDescriptor.h>

struct AnimationGraphDescriptorManager
{
    TP_NOCOPYMOVE(AnimationGraphDescriptorManager);

    static AnimationGraphDescriptorManager& Get() noexcept;
    const AnimationGraphDescriptor* GetDescriptor(uint64_t aKey) const noexcept;


    const TiltedPhoques::Map<uint64_t, AnimationGraphDescriptor> GetDescriptors() const noexcept;
    void UpdateKey(uint64_t aKey, uint64_t newKey) noexcept;
    void Update(uint64_t aKey, uint64_t newKey, AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept;

    struct Builder
    {
        Builder(AnimationGraphDescriptorManager& aManager, uint64_t aKey, AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept;
    };

protected:
    void Register(uint64_t aKey, AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept;

private:
    AnimationGraphDescriptorManager() noexcept;

    TiltedPhoques::Map<uint64_t, AnimationGraphDescriptor> m_descriptors;
};
