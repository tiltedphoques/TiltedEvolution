#pragma once

#include <Misc/BSString.h>
#include "Shared/Sky/SkyObject.h"

class NiBillboardNode;
class BSTriShape;

class Moon final : public SkyObject
{
    friend class Sky;

  public:
    enum UpdateStatus : int
    {
        US_NOT_REQUIRED,
        US_WHEN_CULLED,
        US_INITIALIZE,
    };

    enum Phase : int
    {
        PH_FULL = 0x0,
        PH_3Q_WAN = 0x1,
        PH_HALF_WAN = 0x2,
        PH_1Q_WAN = 0x3,
        PH_NEW = 0x4,
        PH_1Q_WAX = 0x5,
        PH_HALF_WAX = 0x6,
        PH_3Q_WAX = 0x7,
        NUM_PHASES = 0x8,
    };

    explicit Moon(const char* apTextureName, float afAngleFadeStart, float afAngleFadeEnd, float afAngleShadowEarlyFade,
                  float afSpeed, float afZOffset, uint32_t aiSize);

    void Init(NiNode* apRoot);
    void Update(Sky* apSky, float aTick) override;

    static bool UpdatePhase(Sky* apSky);

  private:
    NiPointer<NiBillboardNode> spMoonBaseNode;
    NiPointer<NiNode> spMoonNode;
    NiPointer<NiNode> spShadowNode;
    NiPointer<BSTriShape> spMoonMesh;
    NiPointer<BSTriShape> spShadowMesh;
    BSString strTextures[8];
    float fAngleFadeStart;
    float fAngleFadeEnd;
    float fAngleShadowEarlyFade;
    float fSpeed;
    float fZOffset;
    uint32_t iSize;
    UpdateStatus eUpdateMoonTexture;

    static Phase eCurrentPhase;
};

static_assert(sizeof(Moon) == 0xD8);
