#include <Messages/NotifyPlayerPartyLeft.h>
#include <TiltedCore/Serialization.hpp>

void NotifyPlayerPartyLeft::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, LeavingPlayerId);
}

void NotifyPlayerPartyLeft::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    LeavingPlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;;
}
