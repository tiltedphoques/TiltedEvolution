#include <Structs/Scripts.h>
#include <Serialization.hpp>

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
    Serialization::WriteVarInt(aWriter, Data.size());
    aWriter.WriteBytes(Data.data(), Data.size());
}

void Scripts::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    const auto dataLength = Serialization::ReadVarInt(aReader);
    Data.resize(dataLength);
    aReader.ReadBytes(Data.data(), dataLength);
}
