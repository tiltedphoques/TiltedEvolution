#include <Messages/CancelAssignmentRequest.h>

void CancelAssignmentRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Cookie);
}

void CancelAssignmentRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Cookie = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
}
