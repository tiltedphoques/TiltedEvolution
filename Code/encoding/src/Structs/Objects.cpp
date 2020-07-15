#include <Structs/Objects.h>
#include <Serialization.hpp>

using TiltedPhoques::Serialization;

bool Objects::operator==(const Objects& acRhs) const noexcept
{
    return Data == acRhs.Data;
}

bool Objects::operator!=(const Objects& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void Objects::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Data.size());
    aWriter.WriteBytes(Data.data(), Data.size());
}

void Objects::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    const auto dataLength = Serialization::ReadVarInt(aReader);
    Data.resize(dataLength);
    aReader.ReadBytes(Data.data(), dataLength);
}
