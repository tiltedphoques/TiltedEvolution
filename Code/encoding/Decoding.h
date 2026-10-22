#pragma once

#include <TiltedCore/Buffer.hpp>
#include <TiltedCore/Serialization.hpp>
#include <TiltedCore/Stl.hpp>

// TiltedCore's reader has no error state: reads past the end return zeros. A decoder that finds malformed data
// moves the cursor past the end, which reads never do, and the message factories then drop the message.
namespace Decoding
{
inline bool IsMalformed(const TiltedPhoques::Buffer::Reader& acReader) noexcept
{
    return acReader.GetBitPosition() > acReader.GetBuffer()->GetSize() * 8;
}

inline void MarkMalformed(TiltedPhoques::Buffer::Reader& aReader) noexcept
{
    if (IsMalformed(aReader))
        return;

    const size_t cRemainingBits = aReader.GetBuffer()->GetSize() * 8 - aReader.GetBitPosition();
    aReader.Advance(cRemainingBits / 8 + 1);
}

// Reads a length-prefixed list. A length above aMax, or more elements than the data holds, marks the reader
// malformed. Wire lengths are never used to allocate up front.
template <class T> bool ReadList(TiltedPhoques::Buffer::Reader& aReader, TiltedPhoques::Vector<T>& aOut, uint64_t aMax) noexcept
{
    aOut.clear();

    const uint64_t cCount = TiltedPhoques::Serialization::ReadVarInt(aReader);
    if (cCount > aMax)
    {
        MarkMalformed(aReader);
        return false;
    }

    for (uint64_t i = 0; i < cCount; ++i)
    {
        // Every element takes at least one bit, so running out of data means the list was truncated.
        if (aReader.Eof())
        {
            MarkMalformed(aReader);
            aOut.clear();
            return false;
        }

        T item{};
        item.Deserialize(aReader);
        aOut.push_back(std::move(item));
    }

    if (IsMalformed(aReader))
    {
        aOut.clear();
        return false;
    }

    return true;
}
} // namespace Decoding
