#pragma once

#include "Shared/Sky/SkyObject.h"

class NiTexture;

struct NiColor
{
    float a, b, c, d;
};

class Clouds final : public SkyObject
{
    friend class Sky;

  public:
    Clouds() = default;
    virtual ~Clouds() = default;

    enum
    {
        kTotalLayers = 32
    };
  private:
      #if 0
    NiPointer<BSGeometry> spClouds[32];
    char pad32[0x210 - 32 - 8];
    NiColor pColors[kTotalLayers];
    float fAlpha[kTotalLayers];
    uint16_t usNumLayers;
    bool bForceUpdate;
    std::uint8_t pad513;  // 513
    std::uint32_t pad514; // 514
    std::uint64_t pad520; // 520
    #endif
    char pad0[0x520 - 8];
};

//static_assert(sizeof(Clouds) == 0x520);
