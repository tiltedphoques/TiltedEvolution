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

const TiltedPhoques::Map<uint64_t, AnimationGraphDescriptor> AnimationGraphDescriptorManager::GetDescriptors() const noexcept
{
    return m_descriptors;
}

void AnimationGraphDescriptorManager::UpdateKey(uint64_t aKey, uint64_t newKey) noexcept
{
    auto it = m_descriptors.find(aKey);
    if (it != m_descriptors.end())
    {
        m_descriptors[newKey] = it->second;
        m_descriptors.erase(it);
    }
}

void AnimationGraphDescriptorManager::Update(uint64_t aKey, uint64_t newKey, AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept
{
    const auto it = m_descriptors.find(aKey);
    if (it != std::end(m_descriptors))
    {
        m_descriptors.insert_or_assign(newKey, aAnimationGraphDescriptor);
        m_descriptors.erase(it);
    }
}

AnimationGraphDescriptorManager::Builder::Builder(AnimationGraphDescriptorManager& aManager, uint64_t aKey, AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept
{
    aManager.Register(aKey, std::move(aAnimationGraphDescriptor));
}

void AnimationGraphDescriptorManager::Register(uint64_t aKey, AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept
{
    if (m_descriptors.count(aKey))
        return;

    m_descriptors[aKey] = std::move(aAnimationGraphDescriptor);
}
