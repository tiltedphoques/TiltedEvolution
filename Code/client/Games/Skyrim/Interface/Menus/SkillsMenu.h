#pragma once

#include <Games/Events.h>
#include <Interface/IMenu.h>

class MenuOpenCloseEvent;

class MenuEventHandler
{
  public:
    char pad0[0x10];
};

class StatsMenu final : public IMenu, public MenuEventHandler
{
  public:
    // TBD
};
