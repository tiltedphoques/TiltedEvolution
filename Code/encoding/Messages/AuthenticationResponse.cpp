#include <Messages/AuthenticationResponse.h>

void AuthenticationResponse::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, static_cast<uint32_t>(Type));
    Serialization::WriteString(aWriter, Version);
    UserMods.Serialize(aWriter);
}

void AuthenticationResponse::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    Type = static_cast<ResponseType>(Serialization::ReadVarInt(aReader) & 0xFFFFFFFF);
    Version = Serialization::ReadString(aReader);
    UserMods.Deserialize(aReader);
}
