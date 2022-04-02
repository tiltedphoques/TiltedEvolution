#pragma once

#include "Shared/Sky/SkyObject.h"

class BSFogProperty;
class BSTriShape;

class Atmosphere final : public SkyObject
{
  public:
    Atmosphere() = default;
    virtual ~Atmosphere() = default;

    void Init(NiNode* apRoot, BSFogProperty* apFogProperty);
    void Update(Sky* apSky, float aTick) override;

  private:
    NiPointer<BSTriShape> spAtmosphere;
    NiPointer<BSFogProperty> spFog;
    NiPointer<NiNode> spSkyQuadNode;
    NiPointer<BSTriShape> spSkyQuad;
    bool bUpdateFogDistance;

    static bool bOverrideFogDistance;
};

static_assert(sizeof(Atmosphere) == 0x38);
