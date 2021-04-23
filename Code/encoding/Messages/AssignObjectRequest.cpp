#include <Messages/AssignObjectRequest.h>

void AssignObjectRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Id.Serialize(aWriter);
    CellId.Serialize(aWriter);
    CurrentLockdata.Serialize(aWriter);
}

void AssignObjectRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Id.Deserialize(aReader);
    CellId.Deserialize(aReader);
    CurrentLockdata.Deserialize(aReader);
}
