#pragma once

#include "ExtraData.h"


struct AlchemyItem;
struct EnchantmentItem;

struct ExtraDataList
{
    static ExtraDataList* New() noexcept;
    float GetHealthPercent() const noexcept;
    void SetHealthPercent(float aHealth) noexcept;

    bool Contains(ExtraDataType aType) const;
    void Set(ExtraDataType aType, bool aSet);

    bool Add(ExtraDataType aType, BSExtraData* apNewData);
    bool Remove(ExtraDataType aType, BSExtraData* apNewData);

    uint32_t GetCount() const;

    void SetType(ExtraDataType aType, bool aClear);
    BSExtraData* GetByType(ExtraDataType type) const;


    [[nodiscard]] bool HasQuestObjectAlias() noexcept;

    // Fallout 4 BaseExtraList: singly linked extras plus a presence bitfield.
    BSExtraData* GetHead() const noexcept { return head; }

    uint32_t refCount;        // 00 BSIntrusiveRefCounted
    BSExtraData* head{};      // 08
    BSExtraData** tail{};     // 10
    uint8_t* presence{};      // 18
    mutable BSReadWriteLock lock{}; // 20
};
static_assert(offsetof(ExtraDataList, head) == 0x8);
static_assert(sizeof(ExtraDataList) == 0x28);

inline BSExtraData* ExtraDataList::GetByType(ExtraDataType aType) const
{
    const auto type = static_cast<uint32_t>(aType);
    if (!presence || !(presence[type / 8] & (1 << (type % 8))))
        return nullptr;
    for (auto* pData = head; pData; pData = pData->next)
    {
        if (pData->GetType() == aType)
            return pData;
    }
    return nullptr;
}
