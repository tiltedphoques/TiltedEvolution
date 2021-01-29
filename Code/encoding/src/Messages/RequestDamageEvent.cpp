#include <Messages/RequestDamageEvent.h>
#include <Serialization.hpp>

void RequestDamageEvent::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, m_Id);
    Serialization::WriteFloat(aWriter, m_Damage);
}

void RequestDamageEvent::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    m_Id = Serialization::ReadVarInt(aReader);
    m_Damage = Serialization::ReadFloat(aReader);
}
