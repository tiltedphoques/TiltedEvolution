#include <Messages/NotifyHealthChangeBroadcast.h>
#include <Serialization.hpp>

void NotifyHealthChangeBroadcast::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, m_Id);
    Serialization::WriteFloat(aWriter, m_DeltaHealth);
}

void NotifyHealthChangeBroadcast::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    m_Id = Serialization::ReadVarInt(aReader);
    m_DeltaHealth = Serialization::ReadFloat(aReader);
}
