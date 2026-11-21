#include "NotifyTimeSkip.h"

#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

void NotifyTimeSkip::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteFloat(aWriter, Hours);
    Serialization::WriteVarInt(aWriter, ReadyCount);
    Serialization::WriteVarInt(aWriter, RequiredCount);
}

void NotifyTimeSkip::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    Hours = Serialization::ReadFloat(aReader);
    ReadyCount = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    RequiredCount = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
