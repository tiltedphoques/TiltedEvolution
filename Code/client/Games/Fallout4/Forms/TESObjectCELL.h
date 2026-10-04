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

    enum class CellState : uint8_t
    {
        Attached = 7
    };

    bool IsAttached() const { return cellState == CellState::Attached; }

    struct LoadedCellData
    {
        uint8_t pad0[0x1F0];
        BGSEncounterZone* encounterZone;
    };
    static_assert(offsetof(LoadedCellData, encounterZone) == 0x1F0);

    uint8_t pad20[0x40 - 0x20];
    uint16_t cellFlags;
    uint16_t cellGameFlags;
    CellState cellState;
    bool autoWaterLoaded;
    bool cellDetached;
    uint8_t pad47;
    ExtraDataList* extraData;
    uint64_t cellData;
    void* pCellLand;
    float waterHeight;
    void* pNavMeshes;
    GameArray<TESObjectREFR*> objectList;
    uint8_t pad88[0xC8 - 0x88];
    TESWorldSpace* worldspace;
    LoadedCellData* loadedCellData;
    uint8_t padD8[0xF0 - 0xD8];
};

static_assert(offsetof(TESObjectCELL, cellFlags) == 0x40);
static_assert(offsetof(TESObjectCELL, cellGameFlags) == 0x42);
static_assert(offsetof(TESObjectCELL, cellState) == 0x44);
static_assert(offsetof(TESObjectCELL, objectList) == 0x70);
static_assert(offsetof(TESObjectCELL, worldspace) == 0xC8);
static_assert(offsetof(TESObjectCELL, loadedCellData) == 0xD0);
