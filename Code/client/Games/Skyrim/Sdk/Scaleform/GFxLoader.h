#pragma once

#include "Sdk/Scaleform/GFxResource.h"

namespace Scaleform::GFx
{
struct ExporterInfo
{
    GFxFileConstants_FileFormatType Format;
    const char* pPrefix;
    const char* pSWFName;
    uint16_t Version;

    // Export flag bits.
    enum ExportFlagConstants
    {
        EXF_GlyphTexturesExported = 0x01,
        EXF_GradientTexturesExported = 0x02,
        EXF_GlyphsStripped = 0x10
    };
    ExportFlagConstants ExportFlags;
};
} // namespace Scaleform
