#include <Messages/NotifyActorValuesState.h>
#include <Serialization.hpp>

void NotifyActorValuesState::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, m_Id);
}

void NotifyActorValuesState::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    m_Id = Serialization::ReadVarInt(aReader);
}
