#include <Messages/NotifyInventoryChanges.h>
#include <Serialization.hpp>

void NotifyInventoryChanges::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, Changes.size());

    for (auto& change : Changes)
    {
        Serialization::WriteVarInt(aWriter, change.first);
        Serialization::WriteString(aWriter, change.second);
    }
}

void NotifyInventoryChanges::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    auto count = Serialization::ReadVarInt(aReader) & 0xFF;

    for (auto i = 0u; i < count; ++i)
    {
        auto& change = Changes[Serialization::ReadVarInt(aReader) & 0xFFFFFFFF];
        change = Serialization::ReadString(aReader);
    }
}
