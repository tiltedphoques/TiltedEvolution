#include <Structs/CachedString.h>
#include <TiltedCore/Serialization.hpp>
#include "StringCache.h"

using TiltedPhoques::Serialization;

CachedString& CachedString::operator=(const TiltedPhoques::String& acRhs) noexcept
{
    String::operator=(acRhs);
    return *this;
}

void CachedString::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    const auto cId =  StringCache::Get()[*this];

    Serialization::WriteBool(aWriter, cId.has_value());
    if (cId)
    {
        Serialization::WriteVarInt(aWriter, *cId);
    }
    else
    {
        Serialization::WriteString(aWriter, *this);

        StringCache::Get().AddWanted(*this);
    }
}

void CachedString::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    const auto cHasId = Serialization::ReadBool(aReader);
    if (cHasId)
    {
        const auto cId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
        const auto cValue = StringCache::Get()[cId];
        if (cValue)
        {
            *this = *cValue;
        }
        else
        {
            // If we receive an id but is not in our local cache, then something fucked up
            assert(false);
        }
    }
    else
    {
        *this = Serialization::ReadString(aReader);

        StringCache::Get().AddWanted(*this);
    }
}
