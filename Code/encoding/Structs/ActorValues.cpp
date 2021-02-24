#include <Structs/ActorValues.h>
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

bool ActorValues::operator==(const ActorValues& acRhs) const noexcept
{
    return ActorValuesList == acRhs.ActorValuesList;
}

bool ActorValues::operator!=(const ActorValues& acRhs) const noexcept
{
    return !this->operator==(acRhs);
}

void ActorValues::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, ActorValuesList.size());
    for (auto& value : ActorValuesList)
    {
        Serialization::WriteVarInt(aWriter, value.first);
        Serialization::WriteFloat(aWriter, value.second);
    }

    Serialization::WriteVarInt(aWriter, ActorMaxValuesList.size());
    for (auto& value : ActorMaxValuesList)
    {
        Serialization::WriteVarInt(aWriter, value.first);
        Serialization::WriteFloat(aWriter, value.second);
    }
}

void ActorValues::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    auto count = Serialization::ReadVarInt(aReader);
    for (int i = 0; i < count; i++)
    {
        auto key = Serialization::ReadVarInt(aReader);
        auto value = Serialization::ReadFloat(aReader);
        ActorValuesList.insert({key, value});
    }

    auto maxCount = Serialization::ReadVarInt(aReader);
    for (int i = 0; i < maxCount; i++)
    {
        auto key = Serialization::ReadVarInt(aReader);
        auto value = Serialization::ReadFloat(aReader);
        ActorMaxValuesList.insert({key, value});
    }
}
