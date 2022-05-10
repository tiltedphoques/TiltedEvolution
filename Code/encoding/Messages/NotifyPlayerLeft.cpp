#include <Messages/NotifyPlayerLeft.h>
#include <TiltedCore/Serialization.hpp>

void NotifyPlayerLeft::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ServerId);
    Serialization::WriteString(aWriter, Username);
}

void NotifyPlayerLeft::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    ServerId = Serialization::ReadVarInt(aReader);
    Username = Serialization::ReadString(aReader);
}
