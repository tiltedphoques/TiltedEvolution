#include <Messages/LockChangeRequest.h>

void LockChangeRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Id.Serialize(aWriter);
    CellId.Serialize(aWriter);
    Serialization::WriteVarInt(aWriter, IsLocked);
    Serialization::WriteVarInt(aWriter, LockLevel);
}

void LockChangeRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Id.Deserialize(aReader);
    CellId.Deserialize(aReader);
    IsLocked = Serialization::ReadVarInt(aReader) & 0xFF;
    LockLevel = Serialization::ReadVarInt(aReader) & 0xFF;
}
