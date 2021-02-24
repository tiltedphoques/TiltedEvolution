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
