#pragma once

#include "CachedString.h"

using TiltedPhoques::Buffer;
using TiltedPhoques::Vector;

struct Tints
{
    struct Entry
    {
        CachedString Name;
        float Alpha;
        uint32_t Color;
        uint32_t Type;

        bool operator==(const Entry& acRhs) const noexcept;
        bool operator!=(const Entry& acRhs) const noexcept;
    };

    Tints() = default;
    ~Tints() = default;

    bool operator==(const Tints& acRhs) const noexcept;
    bool operator!=(const Tints& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    Vector<Entry> Entries;
};
