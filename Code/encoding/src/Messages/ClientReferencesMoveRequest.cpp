#include <Messages/ClientReferencesMoveRequest.h>
#include <Serialization.hpp>

void ClientReferencesMoveRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Tick);
    Serialization::WriteVarInt(aWriter, Movements.size());

    for (const auto& kvp : Movements)
    {
        Serialization::WriteVarInt(aWriter, kvp.first);
        kvp.second.Serialize(aWriter);
    }
}

void ClientReferencesMoveRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Tick = Serialization::ReadVarInt(aReader);
    const auto count = Serialization::ReadVarInt(aReader);

    for (auto i = 0u; i < count; ++i)
    {
        auto serverId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
        Movements[serverId].Deserialize(aReader);
    }
}
