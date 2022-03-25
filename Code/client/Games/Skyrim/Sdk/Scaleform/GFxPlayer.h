#pragma once

#include "Sdk/Scaleform/GFxLoaderImpl.h"
#include "Sdk/Scaleform/Kernel/SF_String.h"
#include "Sdk/Scaleform/Render/Render_Twips.h"

namespace Scaleform::GFx
{
class Movie
{
  public:
    enum ScaleModeType
    {
        SM_NoScale = 0x0,
        SM_ShowAll = 0x1,
        SM_ExactFit = 0x2,
        SM_NoBorder = 0x3,
    };
    virtual void SetViewScaleMode(ScaleModeType) = 0;
};
} // namespace Scaleform::GFx
