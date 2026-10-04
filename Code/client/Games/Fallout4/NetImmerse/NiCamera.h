#pragma once

#include <NetImmerse/NiAVObject.h>

struct NiNode;

template <typename T> struct NiRect
{
    T left;
    T right;
    T top;
    T bottom;
};

struct alignas(16) NiCamera : public NiAVObject
{
    virtual ~NiCamera() = default;

    bool WorldPtToScreenPt3(const NiPoint3& in, NiPoint3& out, float zeroTolerance = 1e-5f);

    static bool WorldPtToScreenPt3(float* matrix, const NiRect<float>* port, const NiPoint3* p_in, float* x_out, float* y_out, float* z_out, float zeroTolerance = 1e-5f);

    float worldToCam[4][4];
    uint8_t viewFrustum[0x1C];
    float minNearPlaneDist;
    float maxFarNearRatio;
    NiRect<float> port;
    float lodAdjust;
};

static_assert(offsetof(NiCamera, worldToCam) == 0x120);
static_assert(offsetof(NiCamera, port) == 0x184);
static_assert(sizeof(NiCamera) == 0x1A0);
