#include <Messages/SyncExperienceRequest.h>

void SyncExperienceRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    Serialization::WriteFloat(aWriter, Experience);
}

void SyncExperienceRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    Experience = Serialization::ReadFloat(aReader);
}
