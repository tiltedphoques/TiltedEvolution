#include <Messages/NotifyLockChange.h>

void NotifyLockChange::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Id.Serialize(aWriter);
    Serialization::WriteVarInt(aWriter, IsLocked);
    Serialization::WriteVarInt(aWriter, LockLevel);
}

void NotifyLockChange::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    Id.Deserialize(aReader);
    IsLocked = Serialization::ReadVarInt(aReader) & 0xFF;
    LockLevel = Serialization::ReadVarInt(aReader) & 0xFF;
}
