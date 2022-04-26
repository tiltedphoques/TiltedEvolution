#pragma once

#include <Games/Events.h>
#include <Interface/IMenu.h>

struct MenuOpenCloseEvent;

struct MenuEventHandler
{
    char pad0[0x10];
};

struct StatsMenu final : IMenu, MenuEventHandler
{
    // TBD
};
