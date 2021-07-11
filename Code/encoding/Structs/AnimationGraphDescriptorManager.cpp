#include <Structs/AnimationGraphDescriptorManager.h>
#include <mutex>

#if TP_FALLOUT4
#include <Structs/Fallout4/AnimationGraphDescriptor_Master_Behavior.h>
#else
#include <Structs/Skyrim/AnimationGraphDescriptor_Master_Behavior.h>
#endif

AnimationGraphDescriptorManager::AnimationGraphDescriptorManager() noexcept
{
    static std::once_flag s_initOnce;
    std::call_once(s_initOnce, [this]() 
    {
        AnimationGraphDescriptor_Master_Behavior initAnimationGraphDescriptor_Master_Behavior(*this);
    });
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

AnimationGraphDescriptorManager::Builder::Builder(AnimationGraphDescriptorManager& aManager, const char* acpName,
                                                  AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept
{
    aManager.Register(acpName, std::move(aAnimationGraphDescriptor));
    printf("\t\tGraph descriptor: %s\n", acpName);
}

void AnimationGraphDescriptorManager::Register(const char* acpName, AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept
{
    if (m_descriptors.count(acpName))
        return;

    m_descriptors[acpName] = std::move(aAnimationGraphDescriptor);
}

