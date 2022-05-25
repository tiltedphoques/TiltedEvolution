#pragma once

#include <Games/Primitives.h>

struct NiNode;
struct NiObject;
struct NiCamera;
struct TESCameraState;

struct TESCamera
{
    virtual ~TESCamera() {};

    virtual void SetNode(NiNode* node) {};
    virtual void Update() {};

    NiCamera* GetNiCamera();

    float rotZ;
    float rotX;
    NiPoint3 pos;
    float zoom;
    NiNode* cameraNode;
    TESCameraState* state;
    bool unk;
};
