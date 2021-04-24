#include <Messages/LockChangeRequest.h>

void LockChangeRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Id.Serialize(aWriter);
    CellId.Serialize(aWriter);
    Serialization::WriteBool(aWriter, IsLocked);
    aWriter.WriteBits(LockLevel, 8);
}

void LockChangeRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Id.Deserialize(aReader);
    CellId.Deserialize(aReader);
    IsLocked = Serialization::ReadBool(aReader);

    uint64_t lockLevel = 0;
    aReader.ReadBits(lockLevel, 8);
    LockLevel = lockLevel & 0xFF;
}
