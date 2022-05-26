#pragma once

#include <Camera/TESCamera.h>

struct PlayerCamera : public TESCamera
{
    static PlayerCamera* Get() noexcept;

    bool IsFirstPerson() noexcept;

    bool WorldPtToScreenPt3(const NiPoint3& in, NiPoint3& out, float zeroTolerance = 1e-5f);

    void ForceFirstPerson() noexcept;
    void ForceThirdPerson() noexcept;

    float rotZ;
    float rotX;
    NiPoint3 pos;
    float zoom;
    NiNode* cameraNode;
    TESCameraState* state;
    bool unk;
};
