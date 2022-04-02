#pragma once

#include "NetImmerse/NiPointer.h"

class Sky;
class NiNode;

class SkyObject
{
  public:
    SkyObject() = default;
    virtual ~SkyObject() = default;

    virtual NiNode* GetRoot() = 0;
    virtual void Unk_02() = 0;
    virtual void Update(Sky* apSky, float aTick) = 0;

  private:
    NiPointer<NiNode> m_pRootNode;
};

static_assert(sizeof(SkyObject) == 0x10);
