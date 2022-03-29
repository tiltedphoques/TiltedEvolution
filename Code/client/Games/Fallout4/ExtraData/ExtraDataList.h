#pragma once

#include "ExtraData.h"

struct BaseExtraList
{
    BSExtraData* pHead;
    BSExtraData** ppTail;
    uint8_t* pFlags; // pFlags is 0x1B large
};

struct ExtraDataList : BSIntrusiveRefCounted
{
    static ExtraDataList* New() noexcept;

    bool Contains(ExtraDataType aType) const;
    void Set(ExtraDataType aType, bool aSet);

    bool Add(ExtraDataType aType, BSExtraData* apNewData);
    bool Remove(ExtraDataType aType, BSExtraData* apNewData);

    uint32_t GetCount() const;

    void SetType(ExtraDataType aType, bool aClear);
    BSExtraData* GetByType(ExtraDataType type) const;

    BaseExtraList ExtraData;
    mutable BSRecursiveLock ExtraRWLock; // is BSReadWriteLock
};
