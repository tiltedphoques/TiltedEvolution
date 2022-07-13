#pragma once

#include <Forms/TESForm.h>

struct TESObjectREFR;
struct TESWorldSpace;
struct BGSEncounterZone;
struct LoadedCellData;

struct TESObjectCELL : TESForm
{
    Vector<TESObjectREFR*> GetRefsByFormTypes(const Vector<FormType>& aFormTypes) const noexcept;
    void GetCOCPlacementInfo(NiPoint3* aOutPos, NiPoint3* aOutRot, bool aAllowCellLoad) noexcept;

    uint8_t pad20[0x40 - 0x20];
    uint8_t cellFlags[5];
    uint8_t pad45[0x88 - 0x45];

    struct ReferenceData
    {
        struct Reference
        {
            TESObjectREFR* ref;
            void* unk08;

            TESObjectREFR* Get()
            {
                return unk08 != nullptr ? ref : nullptr;
            }
        };

        uint32_t unk0;
        uint32_t capacity;
        uint32_t available;
        uint32_t unkC;
        void* unk10;
        void* unk18;
        Reference* refArray;

        uint32_t Count()
        {
            return capacity - available;
        }


    };

    
    ReferenceData refData;
    uint8_t unkB0[0x118 - 0xB0];
    BSRecursiveLock lock;

    TESWorldSpace* worldspace;

    struct LoadedCellData
    {
        uint8_t pad0[0x160];
        BGSEncounterZone* encounterZone;
    };

    static_assert(offsetof(LoadedCellData, encounterZone) == 0x160);

    LoadedCellData* loadedCellData;


    bool IsValid() const
    {
        return cellFlags[4] == 7;
    }
};

static_assert(offsetof(TESObjectCELL, cellFlags) == 0x40);
static_assert(offsetof(TESObjectCELL, refData) == 0x88);
static_assert(offsetof(TESObjectCELL, worldspace) == 0x120);
static_assert(offsetof(TESObjectCELL, loadedCellData) == 0x128);
