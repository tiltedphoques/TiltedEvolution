#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Forms/TESForm.h>

struct TESObjectREFR;
struct TESWorldSpace;

struct TESObjectCELL : TESForm
{
#if TP_PLATFORM_64
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
    uint8_t unkB0[0x120 - 0xB0];
    TESWorldSpace* worldspace;

#else
    uint8_t pad20[0x2C - 0x14];
    uint8_t cellFlags[5];
    uint8_t pad31[0x4C - 0x31];

    GameArray<TESObjectREFR*> objects;

    uint8_t pad58[0x84 - 0x58];
    TESWorldSpace* worldspace;
#endif

    bool IsValid() const
    {
        return cellFlags[4] == 7;
    }
};

#if TP_PLATFORM_64
static_assert(offsetof(TESObjectCELL, cellFlags) == 0x40);
static_assert(offsetof(TESObjectCELL, refData) == 0x88);
static_assert(offsetof(TESObjectCELL, worldspace) == 0x120);
#else
static_assert(offsetof(TESObjectCELL, cellFlags) == 0x2C);
static_assert(offsetof(TESObjectCELL, objects) == 0x4C);
static_assert(offsetof(TESObjectCELL, worldspace) == 0x84);
#endif

#endif
