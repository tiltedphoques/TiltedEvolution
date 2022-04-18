#include <Messages/TeleportCommandRequest.h>

void TeleportCommandRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteString(aWriter, TargetPlayer);
}

void TeleportCommandRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    TargetPlayer = Serialization::ReadString(aReader);
}
