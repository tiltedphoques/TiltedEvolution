#include <Messages/RequestActorValuesState.h>
#include <Serialization.hpp>

void RequestActorValuesState::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, m_Id);
}

void RequestActorValuesState::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    m_Id = Serialization::ReadVarInt(aReader);
}
