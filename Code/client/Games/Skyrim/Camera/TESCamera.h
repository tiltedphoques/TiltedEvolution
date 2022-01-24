#pragma once

#include <Games/Primitives.h>

struct NiNode;
struct NiObject;
struct NiCamera;
struct TESCameraState;

struct TESCamera
{
    virtual ~TESCamera() = default;

    virtual void SetNode(NiNode* node) = 0;
    virtual void Update() = 0;

    NiCamera* GetNiCamera();

    float rotZ;
    float rotX;
    NiPoint3 pos;
    float zoom;
    NiNode* cameraNode;
    TESCameraState* state;
    bool unk;
};
