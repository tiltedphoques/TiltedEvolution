
#include <Messages/ServerTimeSettings.h>

void ServerTimeSettings::SerializeRaw(TiltedPhoques::Buffer::Writer &aWriter) const noexcept
{
    // poor man's std::bitcast
    aWriter.WriteBits(*reinterpret_cast<const uint32_t*>(&TimeScale), 32);
    aWriter.WriteBits(*reinterpret_cast<const uint32_t*>(&Time), 32);
}

void ServerTimeSettings::DeserializeRaw(TiltedPhoques::Buffer::Reader &aReader) noexcept
{
    uint64_t tmp = 0;
    uint32_t cVal = 0;

    aReader.ReadBits(tmp, 32);
    cVal = tmp & 0xFFFFFFFF;
    TimeScale = *reinterpret_cast<float*>(&cVal);

    aReader.ReadBits(tmp, 32);
    cVal = tmp & 0xFFFFFFFF;
    Time = *reinterpret_cast<float*>(&cVal);
}
