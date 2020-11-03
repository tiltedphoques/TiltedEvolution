#pragma once

#include <memory>

struct Map;
struct Cell
{
    using Pointer = std::shared_ptr<Cell>;

    Cell();
};
