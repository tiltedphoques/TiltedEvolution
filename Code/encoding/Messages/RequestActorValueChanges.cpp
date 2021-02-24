#include <Messages/RequestActorValueChanges.h>

void RequestActorValueChanges::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Id);

    Serialization::WriteVarInt(aWriter, Values.size());
    for (auto& value : Values)
    {
        Serialization::WriteVarInt(aWriter, value.first);
        Serialization::WriteFloat(aWriter, value.second);
    }
}

void RequestActorValueChanges::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Id = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;

    auto count = Serialization::ReadVarInt(aReader);
    for (int i = 0; i < count; i++)
    {
        auto key = Serialization::ReadVarInt(aReader);
        auto value = Serialization::ReadFloat(aReader);
        Values.insert({key, value});
    }
}
