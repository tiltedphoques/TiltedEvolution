#pragma once

#include <Games/Primitives.h>
#include <NetImmerse/NiPointer.h>

struct NiNode;
struct NiObject;
struct NiCamera;

struct TESCameraState
{
    enum Id : uint32_t
    {
        kFirstPerson = 0,
        kAutoVanity,
        kVATS,
        kFree,
        kIronSights,
        kFurniture,
        kPCTransition,
        kTween,
        kAnimated,
        kThirdPerson,
        kMount,
        kBleedout,
        kDragon,
    };

    uint8_t pad0[0x18];
    Id id; // 18
};

static_assert(offsetof(TESCameraState, id) == 0x18);

struct TESCamera
{
    virtual ~TESCamera(){};

    virtual void SetNode(NiNode* node){};
    virtual void Update(){};

    NiCamera* GetNiCamera();

    float rotZ;
    float rotX;
    NiPoint3 pos;
    float zoom;
    NiPointer<NiNode> cameraRoot;
    TESCameraState* currentState;
    bool unk;
};

static_assert(offsetof(TESCamera, cameraRoot) == 0x20);
static_assert(offsetof(TESCamera, currentState) == 0x28);
