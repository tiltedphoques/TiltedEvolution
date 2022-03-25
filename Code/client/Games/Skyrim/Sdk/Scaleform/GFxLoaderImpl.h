#pragma once

#include "Sdk/Scaleform/GArray.h"
#include "Sdk/Scaleform/GFxLoader.h"
#include "Sdk/Scaleform/Kernel/SF_String.h"
#include "Sdk/Scaleform/Render/Render_Types2D.h"

namespace Scaleform::GFx
{
struct ExporterInfoImpl
{
    ExporterInfo SI;
    Scaleform::String Prefix;
    Scaleform::String SWFName;
    Scaleform::Array<uint32_t, 2> CodeOffsets;
};

struct MovieHeaderData
{
    uint32_t FileLength;
    int Version;
    Render::RectF FrameRect;
    float FPS;
    unsigned FrameCount;
    unsigned SWFFlags;
    ExporterInfoImpl mExporterInfo;
};

} // namespace Scaleform::GFx
