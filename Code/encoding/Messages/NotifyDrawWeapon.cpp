#include <Messages/NotifyDrawWeapon.h>

void NotifyDrawWeapon::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Id);
    Serialization::WriteBool(aWriter, IsWeaponDrawn);
}

void NotifyDrawWeapon::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    Id = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    IsWeaponDrawn = Serialization::ReadBool(aReader);
}
