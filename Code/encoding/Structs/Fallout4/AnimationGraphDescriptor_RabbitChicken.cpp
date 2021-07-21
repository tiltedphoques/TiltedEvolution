#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_RabbitChicken.h>

enum Variables
{
    kiSyncTurnState = 0,
    kiSyncIdleLocomotion = 1,
    kiLocomotionState = 2,
    kSpeed = 3,
    kRunSpeedMult = 4,
    kWalkSpeedMult = 5,
    kTurnDelta = 6,
    kbGraphDriven = 7,
    kTurnDeltaSmoothed = 8,
};

AnimationGraphDescriptor_RabbitChicken::AnimationGraphDescriptor_RabbitChicken(
    AnimationGraphDescriptorManager& aManager)
{
    size_t key = 7023479494570496715;

    AnimationGraphDescriptorManager::Builder s_builder(aManager, key,
        AnimationGraphDescriptor(
            {
                9999
            },
            {
                9999
            },
            {
                9999
            }));
}
