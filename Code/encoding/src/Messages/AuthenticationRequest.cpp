#include <Messages/AuthenticationRequest.h>

void AuthenticationRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteString(aWriter, Token);

    Mods.Serialize(aWriter);
}

void AuthenticationRequest::SerializeDifferential(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
}

void AuthenticationRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Token = Serialization::ReadString(aReader);
    Mods.Deserialize(aReader);
}

void AuthenticationRequest::DeserializeDifferential(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
}

