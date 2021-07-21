#include <Structs/AnimationGraphDescriptorManager.h>
#include <mutex>

#include <Structs/Fallout4/AnimationGraphDescriptor_Master_Behavior.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_CaveCricketRoot.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_RootState.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_SuperMutantRootBehavior.h>

AnimationGraphDescriptorManager::AnimationGraphDescriptorManager() noexcept
{
    static std::once_flag s_initOnce;
    std::call_once(s_initOnce, [this]() {
        AnimationGraphDescriptor_Master_Behavior initAnimationGraphDescriptor_Master_Behavior(*this);
        AnimationGraphDescriptor_CaveCricketRoot initAnimationGraphDescriptor_CaveCricketRoot(*this);
        AnimationGraphDescriptor_RootState initAnimationGraphDescriptor_RootState(*this);
        AnimationGraphDescriptor_SuperMutantRootBehavior initAnimationGraphDescriptor_SuperMutantRootBehavior(*this);
    });
}
