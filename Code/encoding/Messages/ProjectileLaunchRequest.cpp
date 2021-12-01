#include <Messages/ProjectileLaunchRequest.h>

void ProjectileLaunchRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ShooterID);

    Serialization::WriteFloat(aWriter, OriginX);
    Serialization::WriteFloat(aWriter, OriginY);
    Serialization::WriteFloat(aWriter, OriginZ);

    ProjectileBaseID.Serialize(aWriter);
    WeaponID.Serialize(aWriter);
    AmmoID.Serialize(aWriter);

    Serialization::WriteFloat(aWriter, ZAngle);
    Serialization::WriteFloat(aWriter, XAngle);
    Serialization::WriteFloat(aWriter, YAngle);
    
    ParentCellID.Serialize(aWriter);
    SpellID.Serialize(aWriter);

    Serialization::WriteVarInt(aWriter, CastingSource);

    Serialization::WriteVarInt(aWriter, Area);
    Serialization::WriteFloat(aWriter, Power);
    Serialization::WriteFloat(aWriter, Scale);

    Serialization::WriteBool(aWriter, AlwaysHit);
    Serialization::WriteBool(aWriter, NoDamageOutsideCombat);
    Serialization::WriteBool(aWriter, AutoAim);
    Serialization::WriteBool(aWriter, DeferInitialization);
    Serialization::WriteBool(aWriter, ForceConeOfFire);

    // Skyrim
    Serialization::WriteBool(aWriter, UnkBool1);
    Serialization::WriteBool(aWriter, UnkBool2);

    // Fallout 4
    Serialization::WriteFloat(aWriter, ConeOfFireRadiusMult);
    Serialization::WriteBool(aWriter, Tracer);
    Serialization::WriteBool(aWriter, IntentionalMiss);
    Serialization::WriteBool(aWriter, Allow3D);
    Serialization::WriteBool(aWriter, Penetrates);
    Serialization::WriteBool(aWriter, IgnoreNearCollisions);
}

void ProjectileLaunchRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    ShooterID = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;

    OriginX = Serialization::ReadFloat(aReader);
    OriginY = Serialization::ReadFloat(aReader);
    OriginZ = Serialization::ReadFloat(aReader);

    ProjectileBaseID.Deserialize(aReader);
    WeaponID.Deserialize(aReader);
    AmmoID.Deserialize(aReader);

    ZAngle = Serialization::ReadFloat(aReader);
    XAngle = Serialization::ReadFloat(aReader);
    YAngle = Serialization::ReadFloat(aReader);

    ParentCellID.Deserialize(aReader);
    SpellID.Deserialize(aReader);

    CastingSource = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;

    Area = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Power = Serialization::ReadFloat(aReader);
    Scale = Serialization::ReadFloat(aReader);

    AlwaysHit = Serialization::ReadBool(aReader);
    NoDamageOutsideCombat = Serialization::ReadBool(aReader);
    AutoAim = Serialization::ReadBool(aReader);
    DeferInitialization = Serialization::ReadBool(aReader);
    ForceConeOfFire = Serialization::ReadBool(aReader);

    // Skyrim
    UnkBool1 = Serialization::ReadBool(aReader);
    UnkBool2 = Serialization::ReadBool(aReader);

    // Fallout 4
    ConeOfFireRadiusMult = Serialization::ReadFloat(aReader);
    Tracer = Serialization::ReadBool(aReader);
    IntentionalMiss = Serialization::ReadBool(aReader);
    Allow3D = Serialization::ReadBool(aReader);
    Penetrates = Serialization::ReadBool(aReader);
    IgnoreNearCollisions = Serialization::ReadBool(aReader);
}
