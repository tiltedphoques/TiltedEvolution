#pragma once

#include <Forms/TESForm.h>
#include <Components/TESFullName.h>
#include <Components/TESModelTri.h>
#include <Components/TESModelTextureSwap.h>

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
