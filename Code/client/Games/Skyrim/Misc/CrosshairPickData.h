#pragma once

// What the crosshair points at; Target is the ref the HUD's activate prompt ("E Talk") is for.
struct CrosshairPickData
{
    static CrosshairPickData* Get();

    uint32_t pad0;
    uint32_t Target;      // ObjectRefHandle
    uint32_t TargetActor; // ObjectRefHandle
    uint8_t padC[0x38 - 0xC];
};

static_assert(offsetof(CrosshairPickData, Target) == 0x4);
static_assert(offsetof(CrosshairPickData, TargetActor) == 0x8);
static_assert(sizeof(CrosshairPickData) == 0x38);
