#pragma once

#include <TiltedCore/Buffer.hpp>

using TiltedPhoques::Buffer;

struct GridCellCoords
{
    GridCellCoords() = default;
    GridCellCoords(int32_t aX, int32_t aY) noexcept;
    ~GridCellCoords() = default;

    bool operator==(const GridCellCoords& acRhs) const noexcept;
    bool operator!=(const GridCellCoords& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    int32_t X;
    int32_t Y;
};
