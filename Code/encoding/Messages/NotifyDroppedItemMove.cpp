#include <Messages/NotifyDroppedItemMove.h>

void NotifyDroppedItemMove::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Move.Serialize(aWriter);
}

void NotifyDroppedItemMove::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    Move.Deserialize(aReader);
}
