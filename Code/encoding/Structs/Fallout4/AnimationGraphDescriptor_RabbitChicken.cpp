#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/Fallout4/AnimationGraphDescriptor_RabbitChicken.h>


AnimationGraphDescriptor_RabbitChicken::AnimationGraphDescriptor_RabbitChicken(
    AnimationGraphDescriptorManager& aManager)
{
    enum Variables
    {
        kiSyncTurnState = 0,
        kiSyncIdleLocomotion = 1,
        kiLocomotionState = 2,
        kSpeed = 3,
        krunSpeedMult = 4,
        kWalkSpeedMult = 5,
        kTurnDelta = 6,
        kbGraphDriven = 7,
        kTurnDeltaSmoothed = 8,
    };

    uint64_t key = 16006527083653121093;

    AnimationGraphDescriptorManager::Builder s_builder(aManager, key,
        AnimationGraphDescriptor(
            {
                kbGraphDriven
            },
            {
                kSpeed,
                kTurnDelta,
                kWalkSpeedMult,
                krunSpeedMult,
                kTurnDeltaSmoothed,
            },
            {
                kiLocomotionState,
                kiSyncIdleLocomotion,
                kiSyncTurnState,
            }
        ));
}
