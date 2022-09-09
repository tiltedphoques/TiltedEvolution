#include "ExtraDataList.h"

ExtraDataList* ExtraDataList::New() noexcept
{
    ExtraDataList* pExtraDataList = Memory::Allocate<ExtraDataList>();

    pExtraDataList->ExtraData.pHead = nullptr;
    pExtraDataList->ExtraData.ppTail = nullptr;
    pExtraDataList->ExtraData.pFlags = static_cast<uint8_t*>(Memory::Allocate(0x1B));
    memset(pExtraDataList->ExtraData.pFlags, 0, 0x1B);

    pExtraDataList->ExtraRWLock.m_counter = pExtraDataList->ExtraRWLock.m_tid = 0;

    return pExtraDataList;
}

bool ExtraDataList::Contains(ExtraDataType aType) const
{
    BSScopedLock<BSRecursiveLock> _(ExtraRWLock);

    if (ExtraData.pFlags)
    {
        const uint32_t value = static_cast<uint32_t>(aType);
        const uint32_t index = value >> 3;

        if (index >= 0x1B)
            return false;

        const uint8_t element = ExtraData.pFlags[index];

        return (element >> (value % 8)) & 1;
    }

    return false;
}

BSExtraData* ExtraDataList::GetByType(ExtraDataType aType) const
{
    BSScopedLock<BSRecursiveLock> _(ExtraRWLock);

    if (!Contains(aType))
        return nullptr;

    BSExtraData* pEntry = ExtraData.pHead;
    while (pEntry != nullptr && pEntry->type != aType)
    {
        pEntry = pEntry->next;
    }

    return pEntry;
}

bool ExtraDataList::Add(ExtraDataType aType, BSExtraData* apNewData)
{
    if (Contains(aType))
        return false;

    BSScopedLock<BSRecursiveLock> _(ExtraRWLock);

    BSExtraData* pNext = ExtraData.pHead;
    ExtraData.pHead = apNewData;
    apNewData->next = pNext;
    SetType(aType, false);

    return true;
}

uint32_t ExtraDataList::GetCount() const
{
    uint32_t count = 0;

    BSExtraData* pNext = ExtraData.pHead;
    while (pNext)
    {
        count++;
        pNext = pNext->next;
    }

    return count;
}

void ExtraDataList::SetType(ExtraDataType aType, bool aClear)
{
    uint32_t index = static_cast<uint8_t>(aType) >> 3;
    uint8_t bitmask = 1 << (static_cast<uint8_t>(aType) % 8);
    uint8_t& flag = ExtraData.pFlags[index];
    if (aClear)
        flag &= ~bitmask;
    else
        flag |= bitmask;
}
