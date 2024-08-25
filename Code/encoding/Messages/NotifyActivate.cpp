#include <Messages/NotifyActivate.h>

void NotifyActivate::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Id.Serialize(aWriter);
    Serialization::WriteVarInt(aWriter, ActivatorId);
    aWriter.WriteBits(PreActivationOpenState, 8);
}

void NotifyActivate::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    Id.Deserialize(aReader);
    ActivatorId = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    uint64_t preActivationOpenState = 0;
    aReader.ReadBits(preActivationOpenState, 8);
    PreActivationOpenState = preActivationOpenState & 0xFF;
}
