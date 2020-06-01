#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Forms/TESForm.h>
#include <Games/Skyrim/Components/TESFullName.h>
#include <Games/Skyrim/Components/TESModelTri.h>
#include <Games/Skyrim/Components/TESModelTextureSwap.h>

struct BGSTextureSet;

struct BGSHeadPart : TESForm
{
    TESFullName fullName;
    TESModelTextureSwap modelTextureSwap;
    uint8_t pad38[4];
    uint32_t type;
    GameArray<void*> unk40;
    BGSTextureSet* textureSet;
    TESModelTri models[3];
    uint32_t unk8C;
    void* unk90;
    BSFixedString name;
};

#endif
