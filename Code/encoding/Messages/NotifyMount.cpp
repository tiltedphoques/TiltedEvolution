#include <Messages/NotifyMount.h>

void NotifyMount::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, RiderId);
    Serialization::WriteVarInt(aWriter, MountId);
}

void NotifyMount::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    RiderId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    MountId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
