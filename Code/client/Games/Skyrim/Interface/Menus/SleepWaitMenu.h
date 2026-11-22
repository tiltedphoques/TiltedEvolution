#pragma once

#include <Interface/IMenu.h>

struct SleepWaitMenu final : IMenu
{
    static bool IsOpen() noexcept;

    // Starts sleeping or waiting for aHours, as if the player confirmed the open menu.
    static void Confirm(float aHours) noexcept;
};
