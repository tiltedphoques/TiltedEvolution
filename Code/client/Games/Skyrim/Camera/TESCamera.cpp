
#include <Camera/TESCamera.h>
#include <NetImmerse/NiNode.h>
#include <TiltedOnlinePCH.h>

#include <RTTI.h>

POINTER_SKYRIMSE(NiRTTI, NiCameraRTTI, 410506);

NiCamera* TESCamera::GetNiCamera()
{
    // usually the first child should be the camera
    for (auto* child : cameraNode->children)
    {
        if (child && child->GetRTTI() == NiCameraRTTI.Get())
            return reinterpret_cast<NiCamera*>(child);
    }

    return nullptr;
}
