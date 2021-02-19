#include <Structs/Scripts.h>
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

bool Scripts::operator==(const Scripts& acRhs) const noexcept
{
    return Data == acRhs.Data;
}

bool Scripts::operator!=(const Scripts& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void Scripts::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    aWriter.WriteBits(Data.size(), 16);
    aWriter.WriteBytes(Data.data(), Data.size());
}

void Scripts::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    uint64_t tmp = 0;
    aReader.ReadBits(tmp, 16);

    Data.resize(tmp & 0xFFFF);
    aReader.ReadBytes(Data.data(), Data.size());
}
