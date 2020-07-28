#include <Structs/Objects.h>
#include <Serialization.hpp>

using TiltedPhoques::Serialization;

bool Objects::IsEmpty() const noexcept
{
    return Data.empty();
}

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
    aWriter.WriteBits(Data.size() & 0xFFFF, 16);
    aWriter.WriteBytes(Data.data(), Data.size());
}

void Objects::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    uint64_t tmp;
    aReader.ReadBits(tmp, 16);

    Data.resize(tmp & 0xFFFF);
    aReader.ReadBytes(Data.data(), Data.size());
}
