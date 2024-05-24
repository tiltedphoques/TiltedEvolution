#include <Structs/TimeModel.h>
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

void TimeModel::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteFloat(aWriter, TimeScale);
    Serialization::WriteFloat(aWriter, Time);
    Serialization::WriteVarInt(aWriter, Day);
    Serialization::WriteVarInt(aWriter, Month);
    Serialization::WriteVarInt(aWriter, Year);
}

void TimeModel::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    TimeScale = Serialization::ReadFloat(aReader);
    Time = Serialization::ReadFloat(aReader);
    Day = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Month = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Year = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
