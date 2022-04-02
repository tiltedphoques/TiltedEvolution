#pragma once

#include "Shared/Sky/SkyObject.h"

class NiBillboardNode;
class BSTriShape;
class NiDirectionalLight;
class BSShaderAccumulator;

class Sun final : public SkyObject
{
  public:
    Sun() = default;
    virtual ~Sun() = default;

  private:
    NiPointer<NiBillboardNode> spSunBaseNode;
    NiPointer<NiBillboardNode> spSunGlareNode;
    NiPointer<BSTriShape> spSunBase;
    NiPointer<BSTriShape> spSunQuery;
    NiPointer<BSTriShape> spSunGlare;
    NiPointer<NiDirectionalLight> spLight;
    NiPointer<NiDirectionalLight> spCloudLight;
    float fGlareScale;
    bool bDoOcclusionTests;
    NiPointer<BSShaderAccumulator> spSunAccumulator;
};

static_assert(sizeof(Sun) == 0x58);
