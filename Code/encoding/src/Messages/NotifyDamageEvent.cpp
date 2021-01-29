#include <Messages/NotifyDamageEvent.h>
#include <Serialization.hpp>

void NotifyDamageEvent::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, m_Id);
    Serialization::WriteFloat(aWriter, m_Damage);
}

void NotifyDamageEvent::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    m_Id = Serialization::ReadVarInt(aReader);
    m_Damage = Serialization::ReadFloat(aReader);
}
