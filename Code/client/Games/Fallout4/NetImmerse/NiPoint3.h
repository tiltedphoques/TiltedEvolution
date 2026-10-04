#pragma once
// FO4 aligns several transforms to 16 bytes (NiPoint3A).
struct alignas(16) NiPoint3A : NiPoint3
{
};
static_assert(sizeof(NiPoint3A) == 0x10);
