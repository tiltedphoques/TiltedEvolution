#include "NotifyTimeSkip.h"

#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

void NotifyTimeSkip::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteFloat(aWriter, Hours);
    Serialization::WriteVarInt(aWriter, ReadyCount);
    Serialization::WriteVarInt(aWriter, PlayerCount);
}

void NotifyTimeSkip::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    Hours = Serialization::ReadFloat(aReader);
    ReadyCount = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    PlayerCount = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
