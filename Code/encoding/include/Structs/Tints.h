#pragma once

#include <Buffer.hpp>
#include <Stl.hpp>

using TiltedPhoques::Buffer;
using TiltedPhoques::String;
using TiltedPhoques::Vector;

struct Tints
{
    struct Entry
    {
        String Name;
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
