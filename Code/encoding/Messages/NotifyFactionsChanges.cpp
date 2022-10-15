#include <Messages/NotifyFactionsChanges.h>
#include <TiltedCore/Serialization.hpp>
#include <cassert>

static const uint64_t kMaxChangeCount = 0x400;

void NotifyFactionsChanges::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    assert(Changes.size() < 0x400);

    Serialization::WriteVarInt(aWriter, Changes.size());

    for (auto& change : Changes)
    {
        Serialization::WriteVarInt(aWriter, change.first);
        change.second.Serialize(aWriter);
    }
}

void NotifyFactionsChanges::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    uint64_t count = Serialization::ReadVarInt(aReader);

    // Early abort as we don't want to allocate a ton of memory
    if (count >= kMaxChangeCount)
        return;

    for (auto i = 0u; i < count; ++i)
    {
        auto& change = Changes[Serialization::ReadVarInt(aReader) & 0xFFFFFFFF];
        change.Deserialize(aReader);
    }
}
