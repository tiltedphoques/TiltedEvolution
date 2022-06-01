#pragma once


#include <Structs/Vector3_NetQuantize.h>
#include <limits>

using TiltedPhoques::Buffer;

struct GridCellCoords
{
    static const int32_t m_gridsToLoad = 5;
    static const int32_t m_gridsToLoadIfDragon = 20;

    static GridCellCoords CalculateGridCellCoords(const Vector3_NetQuantize& aCoords) noexcept;
    static GridCellCoords CalculateGridCellCoords(const float aX, const float aY) noexcept;
    static bool AreGridCellsOverlapping(const GridCellCoords& aCoords1, const GridCellCoords& aCoords2) noexcept;
    static bool IsCellInGridCell(const GridCellCoords& aCell, const GridCellCoords& aGridCell, bool aIsDragon) noexcept;

    GridCellCoords();
    GridCellCoords(int32_t aX, int32_t aY) noexcept;

    bool operator==(const GridCellCoords& acRhs) const noexcept;
    bool operator!=(const GridCellCoords& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    void Reset() noexcept
    {
        X = Y = std::numeric_limits<int32_t>::max();
    }

    int32_t X{};
    int32_t Y{};
};
