#pragma once

#include <TiltedCore/Buffer.hpp>
#include <Structs/GameId.h>

struct LockData
{
    GameId LockId{};
    GameId CellId{};
    uint8_t IsLocked{};
    uint8_t LockLevel{};
};
