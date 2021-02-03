#include <Messages/NotifyPlayerList.h>
#include <TiltedCore/Serialization.hpp>

void NotifyPlayerList::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Players.size());

    for (auto& player : Players)
    {
        Serialization::WriteVarInt(aWriter, player.first);
        Serialization::WriteString(aWriter, player.second);
    }
}

void NotifyPlayerList::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    auto count = Serialization::ReadVarInt(aReader) & 0xFFFF;

    for (auto i = 0u; i < count; ++i)
    {
        uint32_t id = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
        Players[id] = Serialization::ReadString(aReader);
    }
}
