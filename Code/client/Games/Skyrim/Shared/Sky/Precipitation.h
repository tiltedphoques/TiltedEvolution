#pragma once

#include "Shared/Sky/SkyObject.h"
#include <Misc/BSFixedString.h>

class BSGeometry;

class Precipitation final : public SkyObject
{
  public:
    Precipitation() = default;
    virtual ~Precipitation() = default;

  private:
    char pad0[0x60];
    BSFixedString m_waterName;
    char padx[0x8];
    NiPointer<BSGeometry> m_pGeometry1;
    NiPointer<BSGeometry> m_pGemetry2;
    float m_fSize;
    float m_fdensity1;
    float m_funk3;
    float m_fWetness;
};

static_assert(sizeof(Precipitation) == 0xA0);
