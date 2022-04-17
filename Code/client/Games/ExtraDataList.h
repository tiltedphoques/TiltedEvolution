#pragma once

#include <Games/ExtraData.h>
#include <ExtraData/ExtraSoul.h>
#include <ExtraData/ExtraCharge.h>

#include <Forms/AlchemyItem.h>
#include <Forms/EnchantmentItem.h>
#include <Misc/MapMarkerData.h>

struct ExtraDataList
{
    static ExtraDataList* New() noexcept;

    bool Contains(ExtraData aType) const;
    void Set(ExtraData aType, bool aSet);

    bool Add(ExtraData aType, BSExtraData* apNewData);
    bool Remove(ExtraData aType, BSExtraData* apNewData);

    uint32_t GetCount() const;

    void SetType(ExtraData aType, bool aClear);
    BSExtraData* GetByType(ExtraData type) const;

    void SetSoulData(SOUL_LEVEL aSoulLevel) noexcept;
    void SetChargeData(float aCharge) noexcept;
    void SetWorn(bool aWornLeft) noexcept;
    void SetPoison(AlchemyItem* apItem, uint32_t aCount) noexcept;
    void SetHealth(float aHealth) noexcept;
    void SetEnchantmentData(EnchantmentItem* apItem, uint16_t aCharge, bool aRemoveOnUnequip) noexcept;
    void SetMarkerData(MapMarkerData* apMarkerData) noexcept;

#if TP_FALLOUT4
    void* unk0;
#endif
    BSExtraData* data = nullptr;

    struct Bitfield
    {
        uint8_t data[0x18];
    };
#if TP_FALLOUT4
    void* unk10;
#endif

    Bitfield* bitfield{};
    mutable BSRecursiveLock lock{};
};
