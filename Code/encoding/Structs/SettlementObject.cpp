#include <Structs/SettlementObject.h>
#include <cmath>

void SettlementObject::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    using TiltedPhoques::Serialization;
    Serialization::WriteVarInt(aWriter, Id);
    Serialization::WriteVarInt(aWriter, Revision);
    Serialization::WriteVarInt(aWriter, LeaderReference);
    Workshop.Serialize(aWriter);
    Reference.Serialize(aWriter);
    Base.Serialize(aWriter);
    Cell.Serialize(aWriter);
    WorldSpace.Serialize(aWriter);
    for (int i = 0; i < 3; ++i)
        Serialization::WriteFloat(aWriter, Position[i]);
    for (int i = 0; i < 3; ++i)
        Serialization::WriteFloat(aWriter, Rotation[i]);
    Serialization::WriteVarInt(aWriter, Scale);
    aWriter.WriteBits(Removed, 1);
}

void SettlementObject::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    using TiltedPhoques::Serialization;
    Id = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Revision = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    LeaderReference = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Workshop.Deserialize(aReader);
    Reference.Deserialize(aReader);
    Base.Deserialize(aReader);
    Cell.Deserialize(aReader);
    WorldSpace.Deserialize(aReader);
    for (int i = 0; i < 3; ++i)
        Position[i] = Serialization::ReadFloat(aReader);
    for (int i = 0; i < 3; ++i)
        Rotation[i] = Serialization::ReadFloat(aReader);
    Scale = Serialization::ReadVarInt(aReader) & 0xFFFF;
    uint64_t removed{};
    aReader.ReadBits(removed, 1);
    Removed = removed != 0;
}

bool SettlementObject::operator==(const SettlementObject& acOther) const noexcept
{
    return Id == acOther.Id && Revision == acOther.Revision && LeaderReference == acOther.LeaderReference && Workshop == acOther.Workshop && Reference == acOther.Reference &&
           Base == acOther.Base && Cell == acOther.Cell && WorldSpace == acOther.WorldSpace && Position == acOther.Position &&
           Rotation == acOther.Rotation && Scale == acOther.Scale && Removed == acOther.Removed;
}

bool SettlementObject::IsValid() const noexcept
{
    if (!Workshop || !Base || !Cell || Scale == 0 || Scale > 1000 ||
        (LeaderReference && (LeaderReference < 0xFF000000 || Reference)))
        return false;
    for (int i = 0; i < 3; ++i)
        if (!std::isfinite(Position[i]) || std::abs(Position[i]) > 1048576.f || !std::isfinite(Rotation[i]) || std::abs(Rotation[i]) > 10000.f)
            return false;
    return true;
}
