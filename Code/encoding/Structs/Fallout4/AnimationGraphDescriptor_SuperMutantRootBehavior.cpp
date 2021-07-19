#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_SuperMutantRootBehavior.h>

enum Variables
{

};

AnimationGraphDescriptor_SuperMutantRootBehavior::AnimationGraphDescriptor_SuperMutantRootBehavior(
    AnimationGraphDescriptorManager& aManager)
{
    AnimationGraphDescriptorManager::Builder s_builder(aManager, "SuperMutantRootBehavior",
        AnimationGraphDescriptor(
            {
                9999,
            },
            {
                9999,
            },
            {
                9999,
            }));
}
