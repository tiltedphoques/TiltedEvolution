#include <Messages/ServerTimeSettings.h>

void ServerTimeSettings::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    timeModel.Serialize(aWriter);
}

void ServerTimeSettings::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    timeModel.Deserialize(aReader);
}
