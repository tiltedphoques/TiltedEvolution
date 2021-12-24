#include <TiltedOnlinePCH.h>

#include <Games/ExtraData.h>

bool BSExtraDataList::Contains(ExtraData aType) const
{
    if(bitfield)
    {
        const auto value = static_cast<uint32_t>(aType);
        const auto index = value >> 3;

        const auto element = bitfield->data[index];

        return (element >> (value % 8)) & 1;
    }

    return false;
}

BSExtraData* BSExtraDataList::GetByType(ExtraData aType) const
{
    BSScopedLock<BSRecursiveLock> _(lock);

    if (!Contains(aType))
        return nullptr;

    auto pEntry = data;
#if TP_SKYRIM
    while (pEntry != nullptr && pEntry->GetType() != aType)
#else
    while (pEntry != nullptr && pEntry->type != aType)
#endif
    {
        pEntry = pEntry->next;
    }

    return pEntry;
}

bool BSExtraDataList::Add(ExtraData aType, BSExtraData* apNewData)
{
    if (Contains(aType))
        return false;

    // TODO: this sometimes causes a deadlock
    //BSScopedLock<BSRecursiveLock> _(lock);

    BSExtraData* pNext = data;
    data = apNewData;
    apNewData->next = pNext;
    SetType(aType, false);

    return true;
}

uint32_t BSExtraDataList::GetCount() const
{
    uint32_t count = 0;

    BSExtraData* pNext = data;
    while (pNext)
    {
        count++;
        pNext = pNext->next;
    }

    return count;
}

void BSExtraDataList::SetType(ExtraData aType, bool aClear)
{
    uint32_t index = static_cast<uint8_t>(aType) >> 3;
    uint8_t bitmask = 1 << (static_cast<uint8_t>(aType) % 8);
    uint8_t& flag = bitfield->data[index];
    if (aClear)
        flag &= ~bitmask;
    else
        flag |= bitmask;
}

