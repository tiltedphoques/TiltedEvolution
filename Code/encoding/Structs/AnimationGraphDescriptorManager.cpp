#include <Structs/AnimationGraphDescriptorManager.h>

AnimationGraphDescriptorManager& AnimationGraphDescriptorManager::Get() noexcept
{
    static AnimationGraphDescriptorManager s_manager;
    return s_manager;
}

const AnimationGraphDescriptor* AnimationGraphDescriptorManager::GetDescriptor(std::pair<size_t,size_t> aKey) const noexcept
{
    const auto it = m_descriptors.find(aKey);
    if (it != std::end(m_descriptors))
        return &it->second;

    return nullptr;
}

AnimationGraphDescriptorManager::Builder::Builder(AnimationGraphDescriptorManager& aManager, std::pair<size_t,size_t> aKey,
                                                  AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept
{
    auto keyHash = std::hash<std::pair<size_t, size_t>>{}(aKey);
    printf("\t\tGraph descriptor key: %d %d, hash %u\n", aKey.first, aKey.second, keyHash);
    aManager.Register(aKey, std::move(aAnimationGraphDescriptor));
}

void AnimationGraphDescriptorManager::Register(std::pair<size_t,size_t> aKey, AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept
{
    if (m_descriptors.count(aKey))
    {
        printf("\tGraph descriptor with key %d %d already registered!", aKey.first, aKey.second);
        return;
    }

    m_descriptors[aKey] = std::move(aAnimationGraphDescriptor);
}

