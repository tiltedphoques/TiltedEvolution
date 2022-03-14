#include <Messages/NotifyCharacterInventoryChanges.h>
#include <TiltedCore/Serialization.hpp>

void NotifyCharacterInventoryChanges::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ActorId);
    Item.Serialize(aWriter);
}

void NotifyCharacterInventoryChanges::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    ActorId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Item.Deserialize(aReader);
}
