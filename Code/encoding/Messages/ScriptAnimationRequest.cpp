#include <Messages/ScriptAnimationRequest.h>

void ScriptAnimationRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteVarInt(aWriter, FormID);
    Animation.Serialize(aWriter);
    EventName.Serialize(aWriter);
}

void ScriptAnimationRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    FormID = Serialization::ReadVarInt(aReader) & 0xFFFFFFFF;
    Animation.Deserialize(aReader);
    EventName.Deserialize(aReader);
}
