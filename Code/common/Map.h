#pragma once

#include <TiltedCore/Stl.hpp>
#include <Cell.h>

struct Map
{
    using Coordinates = std::pair<int32_t, int32_t>;

    Cell* At(int32_t aX, int32_t aY) noexcept;

private:

    TiltedPhoques::Map<Coordinates, Cell> m_cells;
};
