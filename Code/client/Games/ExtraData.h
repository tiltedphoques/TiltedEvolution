#pragma once

#include <Games/Primitives.h>
#include <ExtraData.h>

struct TESObjectREFR;
struct TESForm;

struct BSExtraData
{
    inline static constexpr auto eExtraData = ExtraData::None;

    virtual ~BSExtraData() = 0;
    virtual ExtraData GetType() const noexcept = 0;

    BSExtraData* next;

#if TP_FALLOUT4
    uint8_t pad10[2];
    ExtraData type;
    TESForm* form;
#endif
};

#if TP_FALLOUT4
static_assert(offsetof(BSExtraData, type) == 0x12);
static_assert(offsetof(BSExtraData, form) == 0x18);
static_assert(sizeof(BSExtraData) == 0x20);
#endif

struct BSExtraDataList
{
    bool Contains(ExtraData aType) const;
    void Set(ExtraData aType, bool aSet);

    bool Add(ExtraData aType, BSExtraData* apNewData);
    bool Remove(ExtraData aType, BSExtraData* apNewData);

    void SetType(ExtraData aType, bool aClear);

    BSExtraData* GetByType(ExtraData type) const;
#if TP_FALLOUT4
    void* unk0;
#endif
    BSExtraData* data;

    struct Bitfield
    {
        uint8_t data[0x18];
    };
#if TP_FALLOUT4
    void* unk10;
#endif

    Bitfield* bitfield;
    mutable BSRecursiveLock lock;
};
