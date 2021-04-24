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
    Serialization::WriteBool(aWriter, IsLocked);
    aWriter.WriteBits(LockLevel, 8);
}

void LockData::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    IsLocked = Serialization::ReadBool(aReader);

    uint64_t lockLevel = 0;
    aReader.ReadBits(lockLevel, 8);
    LockLevel = lockLevel & 0xFF;
}
