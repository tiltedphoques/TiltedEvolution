#pragma once

#include <Components/BaseFormComponent.h>

struct TESModel : BaseFormComponent
{
    virtual ~TESModel();

    BSFixedString cModel;
    void* pTexturesA;
    void* pMaterialsA;
    uint32_t* pAddonsA;
    uint8_t uiNumTextures;
    uint8_t uiNumTexturesSRGB;
    uint8_t uiNumAddons;
    uint8_t NumMaterials;
    uint8_t cFlags;
};

static_assert(sizeof(TESModel) == 0x30);
