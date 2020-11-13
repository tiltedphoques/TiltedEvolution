#include <Messages/NotifyActorValueChanges.h>
#include <Serialization.hpp>

void NotifyActorValueChanges::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, m_formId);
    Serialization::WriteVarInt(aWriter, m_health);
}

void NotifyActorValueChanges::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    auto formId = Serialization::ReadVarInt(aReader);
    auto health = Serialization::ReadVarInt(aReader);

    m_formId = formId;
    m_health = health;

    //std::cout << "Form ID: " << formId << " Health: " << formId << std::endl;
    //std::cout << "Form ID 2: " << m_formId << " Health 2: " << m_health << std::endl;
}
