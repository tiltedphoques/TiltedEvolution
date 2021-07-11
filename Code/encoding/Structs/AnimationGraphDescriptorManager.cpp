#include <Structs/AnimationGraphDescriptorManager.h>

#if TP_FALLOUT4
#include <Structs/Fallout4/AnimationGraphDescriptor_Master_Behavior.h>
#else
#include <Structs/Skyrim/AnimationGraphDescriptor_Master_Behavior.h>
#endif

AnimationGraphDescriptorManager::AnimationGraphDescriptorManager() noexcept
{
    AnimationGraphDescriptor_Master_Behavior initAnimationGraphDescriptor_Master_Behavior;
}

AnimationGraphDescriptorManager& AnimationGraphDescriptorManager::Get() noexcept
{
    static AnimationGraphDescriptorManager s_manager;
    return s_manager;
}

const AnimationGraphDescriptor* AnimationGraphDescriptorManager::GetDescriptor(const char* acpName) const noexcept
{
    const auto itor = m_descriptors.find(acpName);
    if (itor != std::end(m_descriptors))
        return &itor->second;

    return nullptr;
}

AnimationGraphDescriptorManager::Builder::Builder(const char* acpName, AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept
{
    Get().Register(acpName, std::move(aAnimationGraphDescriptor));
}

void AnimationGraphDescriptorManager::Register(const char* acpName, AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept
{
    if (m_descriptors.count(acpName))
        return;

    m_descriptors[acpName] = std::move(aAnimationGraphDescriptor);
}

