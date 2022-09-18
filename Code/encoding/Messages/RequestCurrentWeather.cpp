#include <Messages/RequestCurrentWeather.h>

void RequestCurrentWeather::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
}

void RequestCurrentWeather::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);
}
