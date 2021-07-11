#include <Structs/AnimationGraphDescriptorManager.h>
#include <mutex>

#include <Structs/Skyrim/AnimationGraphDescriptor_Master_Behavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_BHR_Master.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_WolfRootBehavior.h>

AnimationGraphDescriptorManager::AnimationGraphDescriptorManager() noexcept
{
    static std::once_flag s_initOnce;
    std::call_once(s_initOnce, [this]() 
    {
        AnimationGraphDescriptor_Master_Behavior initAnimationGraphDescriptor_Master_Behavior(*this);
        AnimationGraphDescriptor_BHR_Master initAnimationGraphDescriptor_BHR_Master(*this);
        AnimationGraphDescriptor_WolfRootBehavior initAnimationGraphDescriptor_WolfRootBehavior(*this);
    });
}
