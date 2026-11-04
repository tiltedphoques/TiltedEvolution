#include "VoiceFrame.h"
#include <TiltedCore/Serialization.hpp>

using TiltedPhoques::Serialization;

void VoiceFrame::Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept
{
    const size_t size = std::min(Data.size(), kMaxSize);

    aWriter.WriteBits(Sequence, 16);
    Serialization::WriteVarInt(aWriter, size);
    if (size > 0)
        aWriter.WriteBytes(Data.data(), size);
}

bool VoiceFrame::Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    uint64_t sequence = 0;
    aReader.ReadBits(sequence, 16);
    Sequence = static_cast<uint16_t>(sequence);

    const uint64_t size = Serialization::ReadVarInt(aReader);
    Data.clear();
    if (size == 0 || size > kMaxSize)
        return false;

    Data.resize(static_cast<size_t>(size));
    if (!aReader.ReadBytes(Data.data(), Data.size()))
    {
        Data.clear();
        return false;
    }

    return true;
}
