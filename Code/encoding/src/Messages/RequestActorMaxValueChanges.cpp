#include <Messages/RequestActorMaxValueChanges.h>
#include <Serialization.hpp>

void RequestActorMaxValueChanges::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, m_Id);

    Serialization::WriteVarInt(aWriter, m_values.size());
    for (auto& value : m_values)
    {
        Serialization::WriteVarInt(aWriter, value.first);
        Serialization::WriteFloat(aWriter, value.second);
    }
}

void RequestActorMaxValueChanges::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    m_Id = Serialization::ReadVarInt(aReader);

    auto count = Serialization::ReadVarInt(aReader);
    for (int i = 0; i < count; i++)
    {
        auto key = Serialization::ReadVarInt(aReader);
        auto value = Serialization::ReadFloat(aReader);
        m_values.insert({key, value});
    }
}
