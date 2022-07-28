#include <Messages/NotifyPlayerCellChanged.h>
#include <TiltedCore/Serialization.hpp>

void NotifyPlayerCellChanged::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, PlayerId);
    WorldSpaceId.Serialize(aWriter);
    CellId.Serialize(aWriter);
}

void NotifyPlayerCellChanged::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    PlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    WorldSpaceId.Deserialize(aReader);
    CellId.Deserialize(aReader);
}
