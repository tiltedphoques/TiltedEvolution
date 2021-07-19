#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_RootState.h>

enum Variables
{

};

AnimationGraphDescriptor_RootState::AnimationGraphDescriptor_RootState(
    AnimationGraphDescriptorManager& aManager)
{
    AnimationGraphDescriptorManager::Builder s_builder(aManager, "RootState",
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
