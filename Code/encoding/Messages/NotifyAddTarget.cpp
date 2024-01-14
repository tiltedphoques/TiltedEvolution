#include <Messages/NotifyAddTarget.h>

void NotifyAddTarget::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, TargetId);
    SpellId.Serialize(aWriter);
    EffectId.Serialize(aWriter);
    Serialization::WriteFloat(aWriter, Magnitude);
    Serialization::WriteBool(aWriter, ApplyHealPerkBonus);
    Serialization::WriteBool(aWriter, ApplyStaminaPerkBonus);
}

void NotifyAddTarget::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    TargetId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    SpellId.Deserialize(aReader);
    EffectId.Deserialize(aReader);
    Magnitude = Serialization::ReadFloat(aReader);
    ApplyHealPerkBonus = Serialization::ReadBool(aReader);
    ApplyStaminaPerkBonus = Serialization::ReadBool(aReader);
}
