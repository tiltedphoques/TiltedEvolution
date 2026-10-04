#pragma once

#include <Forms/TESForm.h>

struct BGSOutfit : TESForm
{
    GameArray<TESForm*> outfitItems;
};
