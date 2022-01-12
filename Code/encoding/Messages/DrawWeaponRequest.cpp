#include <Messages/DrawWeaponRequest.h>

void DrawWeaponRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Id);
    Serialization::WriteBool(aWriter, IsWeaponDrawn);
}

void DrawWeaponRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Id = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    IsWeaponDrawn = Serialization::ReadBool(aReader);
}
