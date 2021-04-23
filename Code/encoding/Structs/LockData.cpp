#include <Structs/LockData.h>
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

bool LockData::operator==(const LockData& acRhs) const noexcept
{
    return IsLocked == acRhs.IsLocked &&
        LockLevel == acRhs.LockLevel;
}

bool LockData::operator!=(const LockData& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void LockData::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, IsLocked);
    Serialization::WriteVarInt(aWriter, LockLevel);
}

void LockData::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    IsLocked = Serialization::ReadVarInt(aReader) & 0xFF;
    LockLevel = Serialization::ReadVarInt(aReader) & 0xFF;
}
