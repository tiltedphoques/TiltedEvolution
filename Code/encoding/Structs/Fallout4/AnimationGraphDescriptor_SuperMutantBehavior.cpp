#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_SuperMutantBehavior.h>

enum Variables
{

};

AnimationGraphDescriptor_SuperMutantBehavior::AnimationGraphDescriptor_SuperMutantBehavior(
    AnimationGraphDescriptorManager& aManager)
{
    AnimationGraphDescriptorManager::Builder s_builder(aManager, "SuperMutantBehavior",
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
