
#include <Messages/ServerTimeSettings.h>

void ServerTimeSettings::SerializeRaw(TiltedPhoques::Buffer::Writer &aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerTick);
    Serialization::WriteVarInt(aWriter, static_cast<uint32_t>(TimeScale));
    Serialization::WriteVarInt(aWriter, static_cast<uint32_t>(Time));
}

void ServerTimeSettings::DeserializeRaw(TiltedPhoques::Buffer::Reader &aReader) noexcept
{
    ServerTick = Serialization::ReadVarInt(aReader);
    TimeScale = static_cast<float>(Serialization::ReadVarInt(aReader));
    Time = static_cast<float>(Serialization::ReadVarInt(aReader));
}
