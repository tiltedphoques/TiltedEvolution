#pragma once

#include <Forms/TESForm.h>
#include <Games/Primitives.h>

struct TESObjectREFR;
struct TESWorldSpace;

struct TESObjectCELL : TESForm
{
    Vector<TESObjectREFR*> GetRefsByFormTypes(const Vector<FormType>& aFormTypes) noexcept;
    void GetCOCPlacementInfo(NiPoint3* aOutPos, NiPoint3* aOutRot, bool aAllowCellLoad) noexcept;

    uint8_t pad20[0x40 - 0x20];
    uint8_t cellFlags[5];
    uint8_t pad45[0x70 - 0x45];

    GameArray<TESObjectREFR*> objects; // 70

    uint8_t pad88[0xC8 - 0x88];
    TESWorldSpace* worldspace; // C8

    bool IsValid() const
    {
        return cellFlags[4] == 8;
    }
};

static_assert(offsetof(TESObjectCELL, cellFlags) == 0x40);
static_assert(offsetof(TESObjectCELL, objects) == 0x70);
static_assert(offsetof(TESObjectCELL, worldspace) == 0xC8);
