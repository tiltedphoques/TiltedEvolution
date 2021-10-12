#include <Structs/AnimationGraphDescriptorManager.h>
#include <iostream>

AnimationGraphDescriptorManager& AnimationGraphDescriptorManager::Get() noexcept
{
    static AnimationGraphDescriptorManager s_manager;
    return s_manager;
}

const AnimationGraphDescriptor* AnimationGraphDescriptorManager::GetDescriptor(uint64_t aKey) const noexcept
{
    const auto it = m_descriptors.find(aKey);
    if (it != std::end(m_descriptors))
        return &it->second;

    return nullptr;
}

AnimationGraphDescriptorManager::Builder::Builder(AnimationGraphDescriptorManager& aManager, uint64_t aKey,
                                                  AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept
{
    aManager.Register(aKey, std::move(aAnimationGraphDescriptor));
}

void AnimationGraphDescriptorManager::Register(uint64_t aKey, AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept
{
    if (m_descriptors.count(aKey))
        return;

    m_descriptors[aKey] = std::move(aAnimationGraphDescriptor);
}

