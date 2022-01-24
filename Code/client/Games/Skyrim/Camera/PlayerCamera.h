#pragma once

#include <Camera/TESCamera.h>

struct PlayerCamera : public TESCamera
{
    virtual ~PlayerCamera() = default;

    static PlayerCamera* Get() noexcept;

    bool WorldPtToScreenPt3(const NiPoint3& in, NiPoint3& out, float zeroTolerance = 1e-5f);

    float rotZ;
    float rotX;
    NiPoint3 pos;
    float zoom;
    NiNode* cameraNode;
    TESCameraState* state;
    bool unk;
};
