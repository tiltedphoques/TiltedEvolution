#pragma once

#include <mem/mem.h>
#include "SteamCrypto.h"

namespace steam
{
void CrackCEGInPlace(const uint8_t* apStart, const mem::region acText);
}
