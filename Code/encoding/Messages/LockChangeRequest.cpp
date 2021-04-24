#include <Messages/LockChangeRequest.h>

void LockChangeRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Id.Serialize(aWriter);
    CellId.Serialize(aWriter);
    Serialization::WriteBool(aWriter, IsLocked);
    Serialization::WriteBool(aWriter, LockLevel);
}

void LockChangeRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Id.Deserialize(aReader);
    CellId.Deserialize(aReader);
    IsLocked = Serialization::ReadBool(aReader);
    LockLevel = Serialization::ReadBool(aReader);
}
