#include <Structs/AnimationGraphDescriptorManager.h>

AnimationGraphDescriptorManager& AnimationGraphDescriptorManager::Get() noexcept
{
    static AnimationGraphDescriptorManager s_manager;
    return s_manager;
}

const AnimationGraphDescriptor* AnimationGraphDescriptorManager::GetDescriptor(size_t aKey) const noexcept
{
    const auto it = m_descriptors.find(aKey);
    if (it != std::end(m_descriptors))
        return &it->second;

    return nullptr;
}

AnimationGraphDescriptorManager::Builder::Builder(AnimationGraphDescriptorManager& aManager, size_t aKey,
                                                  AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept
{
    printf("\t\tGraph descriptor key: %d\n", aKey);
    aManager.Register(aKey, std::move(aAnimationGraphDescriptor));
}

void AnimationGraphDescriptorManager::Register(size_t aKey, AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept
{
    if (m_descriptors.count(aKey))
    {
        printf("\tGraph descriptor with key %d already registered!", aKey);
        return;
    }

    m_descriptors[aKey] = std::move(aAnimationGraphDescriptor);
}

