#pragma once

#include <NetImmerse/ImageSpaceModifierInstance.h>

struct TESImageSpaceModifier;

struct ImageSpaceModifierInstanceForm : public ImageSpaceModifierInstance
{
    static void Stop(TESImageSpaceModifier* apMod)
    {
        using TClearImageSpaceModifier = void(TESImageSpaceModifier*);
        static VersionDbPtr<TClearImageSpaceModifier> s_clearImageSpaceModifier(2199909);
        return s_clearImageSpaceModifier.Get()(apMod);
    }

    TESImageSpaceModifier* pMod;
    uint64_t unk30;
    float unk38;
    uint32_t unk3C;
    uint64_t unk40;
    uint32_t unk48;
};
