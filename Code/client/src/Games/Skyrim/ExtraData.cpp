#if defined(TP_SKYRIM)

#include <Games/Skyrim/ExtraData.h>

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

#endif
