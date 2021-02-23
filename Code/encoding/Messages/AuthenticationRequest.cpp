#include <Messages/AuthenticationRequest.h>

void AuthenticationRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, DiscordId);
    Serialization::WriteString(aWriter, Token);
    UserMods.Serialize(aWriter);
    Serialization::WriteString(aWriter, Username);
}

void AuthenticationRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    DiscordId = Serialization::ReadVarInt(aReader);
    Token = Serialization::ReadString(aReader);
    UserMods.Deserialize(aReader);
    Username = Serialization::ReadString(aReader);
}
