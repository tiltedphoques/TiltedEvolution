
#include <Messages/ServerSkySeed.h>

void ServerSkySeed::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    aWriter.WriteBits(Seed, 32);
}

void ServerSkySeed::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    uint64_t tmp = 0;
    aReader.ReadBits(tmp, 32);
    Seed = tmp & 0xFFFFFFFF;
}
