#pragma once

#include <Forms/TESForm.h>
#include <Games/ExtraDataList.h>

struct TESObjectREFR;
struct TESWorldSpace;
struct BGSEncounterZone;

struct TESObjectCELL : TESForm
{
    Vector<TESObjectREFR*> GetRefsByFormTypes(const Vector<FormType>& aFormTypes) noexcept;

    uint8_t pad20[0x40 - 0x20];
    uint8_t cellFlags[5];
    uint8_t pad45[0x48 - 0x45];
    ExtraDataList extraDataList;
    uint8_t pad60[0x88 - 0x60];

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

    struct LOADED_CELL_DATA
    {
        uint8_t pad0[0x160];
        BGSEncounterZone* pEncounterZone;
    };

    ReferenceData refData;
    uint8_t unkB0[0x118 - 0xB0];
    BSRecursiveLock lock;
    TESWorldSpace* worldspace;
    LOADED_CELL_DATA* pCellData;

    bool IsValid() const
    {
        return cellFlags[4] == 7;
    }
};

static_assert(offsetof(TESObjectCELL, cellFlags) == 0x40);
static_assert(offsetof(TESObjectCELL, refData) == 0x88);
static_assert(offsetof(TESObjectCELL, worldspace) == 0x120);

