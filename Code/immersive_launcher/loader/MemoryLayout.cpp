
/*
 * This file is part of the CitizenFX project - http://citizen.re/
 *
 * See LICENSE and MENTIONS in the root of the source tree for information
 * regarding licensing.
 */

// Changes:
// - 2021/2/24: Increased game segment size for Fallout4.

#include "TargetConfig.h"
#include <Windows.h>

#pragma comment(linker, "/merge:.data=.cld")
#pragma comment(linker, "/merge:.rdata=.clr")
#pragma comment(linker, "/merge:.cl=.zdata")
#pragma comment(linker, "/merge:.text=.zdata")
#pragma comment(linker, "/section:.zdata,re")

#pragma bss_seg(".game")
uint8_t game_seg[CurrentTarget.exeLoadSz];

// high rip zone, used for RIP relative addressing
// guaranteed to always be there.
uint8_t highrip[0x100000];

#pragma data_seg(".xcode")
uint8_t zdata[200] = {1};

namespace
{
extern "C" const IMAGE_DOS_HEADER __ImageBase;

constinit const uint8_t* pBasePtr = reinterpret_cast<const uint8_t*>(&__ImageBase);
const uint8_t* kpImageEnd{pBasePtr +
                          ((PIMAGE_NT_HEADERS)(pBasePtr + __ImageBase.e_lfanew))->OptionalHeader.SizeOfImage};

bool InRange(const uint8_t* apObj, const uint8_t* apLo, const uint8_t* apHi)
{
    return apObj >= apLo && apObj <= apHi;
};
} // namespace

#define EXP __declspec(dllexport)
bool EXP IsThisExeAddress(const uint8_t* apAddress)
{
    return InRange(apAddress, pBasePtr, kpImageEnd);
}

bool EXP IsGameMemoryAddress(const uint8_t* apAddress)
{
    return InRange(apAddress, &highrip[0], &highrip[sizeof(highrip)]) ||
           InRange(apAddress, &game_seg[0], &game_seg[sizeof(game_seg)]);
}
#undef EXP
