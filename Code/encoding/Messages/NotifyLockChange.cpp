#include <Messages/NotifyLockChange.h>

void NotifyLockChange::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Id.Serialize(aWriter);
    Serialization::WriteBool(aWriter, IsLocked);
    aWriter.WriteBits(LockLevel, 8);
}

void NotifyLockChange::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    Id.Deserialize(aReader);
    IsLocked = Serialization::ReadBool(aReader);

    uint64_t lockLevel = 0;
    aReader.ReadBits(lockLevel, 8);
    LockLevel = lockLevel & 0xFF;
}
