#pragma once

#include "SteamCrypto.h"
#include <mem/mem.h>

namespace steam
{
struct CEGLocationInfo
{
    uint8_t* start;
    mem::region textSeg;
};

uintptr_t CrackCEGInPlace(const CEGLocationInfo&);
} // namespace steam
