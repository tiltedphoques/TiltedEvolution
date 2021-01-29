#include <Messages/RequestHealthChangeBroadcast.h>
#include <Serialization.hpp>

void RequestHealthChangeBroadcast::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, m_Id);
    Serialization::WriteFloat(aWriter, m_DeltaHealth);
}

void RequestHealthChangeBroadcast::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    m_Id = Serialization::ReadVarInt(aReader);
    m_DeltaHealth = Serialization::ReadFloat(aReader);
}
