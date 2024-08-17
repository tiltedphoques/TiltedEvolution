#include "SetTimeCommandRequest.h"

void SetTimeCommandRequest::SerializeRaw(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    aWriter.WriteBits(Hours, 8);
    aWriter.WriteBits(Minutes, 8);
    aWriter.WriteBits(PlayerId, 32);
}

void SetTimeCommandRequest::DeserializeRaw(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    ClientMessage::DeserializeRaw(aReader);

    uint64_t dest = 0;
    aReader.ReadBits(dest, 8);
    Hours = dest & 0xFF;
    aReader.ReadBits(dest, 8);
    Minutes = dest & 0xFF;
    aReader.ReadBits(dest, 32);
    PlayerId = dest & 0xFFFFFFFF;
}
