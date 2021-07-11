#include <Structs/AnimationGraphDescriptorManager.h>
#include <mutex>

//#include <Structs/Fallout4/AnimationGraphDescriptor_Master_Behavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_Master_Behavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_BHR_Master.h>

AnimationGraphDescriptorManager::AnimationGraphDescriptorManager() noexcept
{
    static std::once_flag s_initOnce;
    std::call_once(s_initOnce, [this]() 
    {
        AnimationGraphDescriptor_Master_Behavior initAnimationGraphDescriptor_Master_Behavior(*this);
        AnimationGraphDescriptor_BHR_Master initAnimationGraphDescriptor_BHR_Master(*this);
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
    printf("\t\tGraph descriptor: %s\n", acpName);
    aManager.Register(acpName, std::move(aAnimationGraphDescriptor));
}

void AnimationGraphDescriptorManager::Register(const char* acpName, AnimationGraphDescriptor aAnimationGraphDescriptor) noexcept
{
    if (m_descriptors.count(acpName))
        return;

    m_descriptors[acpName] = std::move(aAnimationGraphDescriptor);
}

