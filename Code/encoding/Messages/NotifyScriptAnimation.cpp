#include <Messages/NotifyScriptAnimation.h>

void NotifyScriptAnimation::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, FormID);
    Animation.Serialize(aWriter);
    EventName.Serialize(aWriter);
}

void NotifyScriptAnimation::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ServerMessage::DeserializeRaw(aReader);

    FormID = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Animation.Deserialize(aReader);
    EventName.Deserialize(aReader);
}
