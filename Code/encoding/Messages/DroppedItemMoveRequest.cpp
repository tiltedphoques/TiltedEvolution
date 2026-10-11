#include <Messages/DroppedItemMoveRequest.h>

void DroppedItemMoveRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Move.Serialize(aWriter);
}

void DroppedItemMoveRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Move.Deserialize(aReader);
}
