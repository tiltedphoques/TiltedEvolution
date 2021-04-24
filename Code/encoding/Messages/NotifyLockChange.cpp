#include <Messages/NotifyLockChange.h>

void NotifyLockChange::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Id.Serialize(aWriter);
    Serialization::WriteBool(aWriter, IsLocked);
    Serialization::WriteBool(aWriter, LockLevel);
}

void NotifyLockChange::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    Id.Deserialize(aReader);
    IsLocked = Serialization::ReadBool(aReader);
    LockLevel = Serialization::ReadBool(aReader);
}
