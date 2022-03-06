
#include <Camera/PlayerCamera.h>
#include <NetImmerse/NiCamera.h>
#include <TiltedOnlinePCH.h>

PlayerCamera* PlayerCamera::Get() noexcept
{
    POINTER_SKYRIMSE(PlayerCamera*, s_instance, 400802);
    return *(s_instance.Get());
}

bool PlayerCamera::WorldPtToScreenPt3(const NiPoint3& in, NiPoint3& out, float zeroTolerance /* = 1e-5f */)
{
    auto* pCam = GetNiCamera();
    if (cameraNode && pCam)
    {
        return pCam->WorldPtToScreenPt3(in, out, zeroTolerance);
    }

    return false;
}
