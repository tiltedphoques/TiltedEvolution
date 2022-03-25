#pragma once

#include "Sdk/Scaleform/GFxLoaderImpl.h"
#include "Sdk/Scaleform/Render/Render_Twips.h"
#include "Sdk/Scaleform/Kernel/SF_String.h"

namespace Scaleform::GFx
{
struct MovieDataDef
{
    struct LoadTaskDataBase
    {
        char pad0[0x38];
    };

    struct LoadTaskData : LoadTaskDataBase
    {
        void* pHeap;
        void* pImageHeap;
        Scaleform::StringLH FileURL;
        MovieHeaderData Header;
    };
};
} // namespace Scaleform::GFx
