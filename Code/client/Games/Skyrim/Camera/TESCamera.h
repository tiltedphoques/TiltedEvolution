#pragma once

#include <Games/Primitives.h>

struct NiNode;
struct NiObject;
struct NiCamera;
struct TESCameraState;

// here for now
template<typename T>
struct BSTPoint2
{
    T x, y;
};

template <typename T> struct BSTPoint3
{
    T x, y, z;
};


struct TESCamera
{
    virtual ~TESCamera(){};

    virtual void SetNode(NiNode* node){};
    virtual void Update(){};

    NiCamera* GetNiCamera();

  public:
    BSTPoint2<float> RotationInput;
    BSTPoint3<float> TranslationInput;
    float fZoomInput;
    NiPointer<NiNode> spCameraRoot;
    TESCameraState* spCurrentState; // actually a smart ptr
    bool bEnabled;
};

static_assert(sizeof(TESCamera) == 0x38);
