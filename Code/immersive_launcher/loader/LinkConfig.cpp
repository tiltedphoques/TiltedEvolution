
/*
 * This file is part of the CitizenFX project - http://citizen.re/
 *
 * See LICENSE and MENTIONS in the root of the source tree for information
 * regarding licensing.
 */

// Changes:
// - 2021/2/24: Increased game segment size for Fallout4.

#pragma comment(linker, "/merge:.data=.cld")
#pragma comment(linker, "/merge:.rdata=.clr")
#pragma comment(linker, "/merge:.cl=.zdata")
#pragma comment(linker, "/merge:.text=.zdata")
#pragma comment(linker, "/section:.zdata,re")

#pragma bss_seg(".game")
char game_seg[0x7000000];

// high rip zone, used for RIP relative addressing
// guaranteed to always be there.
char highrip[0x100000];

#pragma data_seg(".code")
char zdata[200] = { 1 };
