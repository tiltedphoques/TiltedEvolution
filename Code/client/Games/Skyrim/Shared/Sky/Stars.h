#pragma once

#include "Shared/Sky/SkyObject.h"

class Stars final : public SkyObject
{
  public:
    Stars() = default;
    virtual ~Stars() = default;

  private:
    NiPointer<NiNode> m_pStarNode;
    float m_fAlpha = 0.f;
};

static_assert(sizeof(Stars) == 0x20);
