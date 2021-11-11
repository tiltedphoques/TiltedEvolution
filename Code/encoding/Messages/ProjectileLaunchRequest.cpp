#include <Messages/ProjectileLaunchRequest.h>

void ProjectileLaunchRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ShooterID);

    Serialization::WriteFloat(aWriter, OriginX);
    Serialization::WriteFloat(aWriter, OriginY);
    Serialization::WriteFloat(aWriter, OriginZ);
    Serialization::WriteFloat(aWriter, ContactNormalX);
    Serialization::WriteFloat(aWriter, ContactNormalY);
    Serialization::WriteFloat(aWriter, ContactNormalZ);

    ProjectileBaseID.Serialize(aWriter);
    WeaponID.Serialize(aWriter);
    AmmoID.Serialize(aWriter);

    Serialization::WriteFloat(aWriter, ZAngle);
    Serialization::WriteFloat(aWriter, XAngle);
    Serialization::WriteFloat(aWriter, YAngle);
    
    ParentCellID.Serialize(aWriter);
    SpellID.Serialize(aWriter);

    Serialization::WriteVarInt(aWriter, CastingSource);

    Serialization::WriteBool(aWriter, unkBool1);

    Serialization::WriteVarInt(aWriter, Area);
    Serialization::WriteFloat(aWriter, Power);
    Serialization::WriteFloat(aWriter, Scale);

    Serialization::WriteBool(aWriter, AlwaysHit);
    Serialization::WriteBool(aWriter, NoDamageOutsideCombat);
    Serialization::WriteBool(aWriter, AutoAim);
    Serialization::WriteBool(aWriter, UseOrigin);
    Serialization::WriteBool(aWriter, DeferInitialization);
    Serialization::WriteBool(aWriter, Tracer);
    Serialization::WriteBool(aWriter, ForceConeOfFire);
}

void ProjectileLaunchRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    ShooterID = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;

    OriginX = Serialization::ReadFloat(aReader);
    OriginY = Serialization::ReadFloat(aReader);
    OriginZ = Serialization::ReadFloat(aReader);
    ContactNormalX = Serialization::ReadFloat(aReader);
    ContactNormalY = Serialization::ReadFloat(aReader);
    ContactNormalZ = Serialization::ReadFloat(aReader);

    ProjectileBaseID.Deserialize(aReader);
    WeaponID.Deserialize(aReader);
    AmmoID.Deserialize(aReader);

    ZAngle = Serialization::ReadFloat(aReader);
    XAngle = Serialization::ReadFloat(aReader);
    YAngle = Serialization::ReadFloat(aReader);

    ParentCellID.Deserialize(aReader);
    SpellID.Deserialize(aReader);

    CastingSource = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;

    unkBool1 = Serialization::ReadBool(aReader);

    Area = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Power = Serialization::ReadFloat(aReader);
    Scale = Serialization::ReadFloat(aReader);

    AlwaysHit = Serialization::ReadBool(aReader);
    NoDamageOutsideCombat = Serialization::ReadBool(aReader);
    AutoAim = Serialization::ReadBool(aReader);
    UseOrigin = Serialization::ReadBool(aReader);
    DeferInitialization = Serialization::ReadBool(aReader);
    Tracer = Serialization::ReadBool(aReader);
    ForceConeOfFire = Serialization::ReadBool(aReader);
}
