#pragma once

#include <Games/Fallout4/SaveLoad.h>
#include <Games/Skyrim/SaveLoad.h>

struct ScopedSaveLoadOverride
{
    ScopedSaveLoadOverride();
    ~ScopedSaveLoadOverride();

    TP_NOCOPYMOVE(ScopedSaveLoadOverride);
};
