#include <Havok/bhkCharacterController.h>

bool bhkCharacterController::UpdateStepTiming(float aMovementDeltaTime) noexcept
{
    // bhkWorld::fStepTimeS, the step bhkCharRigidBodyController::Init gives new controllers
    static VersionDbPtr<float> s_physicsDeltaTime(2704758);
    return stepInfo.UpdateDeltaTime(*s_physicsDeltaTime.Get(), aMovementDeltaTime);
}
