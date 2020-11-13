#include <Messages/RequestActorValueChanges.h>
#include <Serialization.hpp>
#include <iostream>

void RequestActorValueChanges::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, m_formId);
    Serialization::WriteVarInt(aWriter, m_health);
}

void RequestActorValueChanges::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    auto formId = Serialization::ReadVarInt(aReader);
    auto health = Serialization::ReadVarInt(aReader);
    //Cookie = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    m_formId = formId;
    m_health = health;
}
