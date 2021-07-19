#pragma once

#include <Structs/AnimationGraphDescriptor.h>

using TiltedPhoques::Map;

namespace std
{
    template <> class hash<std::pair<size_t,size_t>>
    {
      public:
        size_t operator()(const std::pair<size_t,size_t>& key) const
        {
            return hash<size_t>()(key.second) ^ (hash<size_t>()(key.first) << 1);
        }
    };
}

struct AnimationGraphDescriptorManager
{
    TP_NOCOPYMOVE(AnimationGraphDescriptorManager);

    static AnimationGraphDescriptorManager& Get() noexcept;
    const AnimationGraphDescriptor* GetDescriptor(std::pair<size_t,size_t> aKey) const noexcept;

    struct Builder
    {
        Builder(AnimationGraphDescriptorManager& aManager, std::pair<size_t,size_t> aKey,
                AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept;
    };

protected:

    void Register(std::pair<size_t,size_t> aKey, AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept;

private:

    AnimationGraphDescriptorManager() noexcept;

    Map<std::pair<size_t,size_t>, AnimationGraphDescriptor> m_descriptors;
};
