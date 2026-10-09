#pragma once

#include <Forms/TESForm.h>
#include <Components/TESFullName.h>

struct BGSTextureSet;

struct BGSHeadPart : TESForm
{
    TESFullName fullName;
    uint8_t pad30[0x40];
    uint8_t flags;
    uint8_t pad71[3];
    uint32_t type;
    GameArray<BGSHeadPart*> extraParts;
    BGSTextureSet* textureSet;
    uint8_t pad98[0xD8];
    BSFixedString name;
};

static_assert(offsetof(BGSHeadPart, type) == 0x74);
static_assert(offsetof(BGSHeadPart, textureSet) == 0x90);
static_assert(offsetof(BGSHeadPart, name) == 0x170);
static_assert(sizeof(BGSHeadPart) == 0x178);
