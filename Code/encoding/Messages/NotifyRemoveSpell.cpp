#include "NotifyRemoveSpell.h"

void NotifyRemoveSpell::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, TargetId);
    SpellId.Serialize(aWriter);
}

void NotifyRemoveSpell::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    TargetId = Serialization::ReadVarInt(aReader);
    SpellId.Deserialize(aReader);
}
