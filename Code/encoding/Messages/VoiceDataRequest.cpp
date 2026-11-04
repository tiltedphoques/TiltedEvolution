#include <Messages/VoiceDataRequest.h>

void VoiceDataRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Frame.Serialize(aWriter);
}

void VoiceDataRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Frame.Deserialize(aReader);
}
