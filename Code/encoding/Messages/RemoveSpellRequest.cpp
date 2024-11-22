#include "EncodingPch.h"
#include "RemoveSpellRequest.h"

void RemoveSpellRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, TargetId);
    SpellId.Serialize(aWriter);
}

void RemoveSpellRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    TargetId = Serialization::ReadVarInt(aReader);
    SpellId.Deserialize(aReader);
}
