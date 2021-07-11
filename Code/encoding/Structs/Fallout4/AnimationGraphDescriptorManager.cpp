#include <Structs/AnimationGraphDescriptorManager.h>
#include <mutex>

#include <Structs/Fallout4/AnimationGraphDescriptor_Master_Behavior.h>

AnimationGraphDescriptorManager::AnimationGraphDescriptorManager() noexcept
{
    static std::once_flag s_initOnce;
    std::call_once(s_initOnce, [this]() {
        AnimationGraphDescriptor_Master_Behavior initAnimationGraphDescriptor_Master_Behavior(*this);
    });
}
