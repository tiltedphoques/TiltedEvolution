#include <Structs/FullObjects.h>
#include <Serialization.hpp>

using TiltedPhoques::Serialization;

bool FullObjects::operator==(const FullObjects& acRhs) const noexcept
{
    return Data == acRhs.Data;
}

bool FullObjects::operator!=(const FullObjects& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void FullObjects::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Data.size());
    aWriter.WriteBytes(Data.data(), Data.size());
}

void FullObjects::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    const auto dataLength = Serialization::ReadVarInt(aReader);
    Data.resize(dataLength);
    aReader.ReadBytes(Data.data(), dataLength);
}
