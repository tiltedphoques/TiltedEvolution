#include <Messages/NotifyPartyInfo.h>
#include <TiltedCore/Serialization.hpp>

void NotifyPartyInfo::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteBool(aWriter, IsLeader);
    Serialization::WriteVarInt(aWriter, LeaderPlayerId);
    aWriter.WriteBits(PlayerIds.size() & 0xFF, 8);

    for (auto player : PlayerIds)
    {
        Serialization::WriteVarInt(aWriter, player);
    }
}

void NotifyPartyInfo::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    IsLeader = Serialization::ReadBool(aReader);
    LeaderPlayerId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;

    uint64_t count = 0;
    aReader.ReadBits(count, 8);

    PlayerIds.resize(count);

    for (auto i = 0u; i < count; ++i)
    {
        PlayerIds[i] = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    }
}
