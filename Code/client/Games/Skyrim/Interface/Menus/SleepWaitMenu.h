#pragma once

namespace SleepWaitMenu
{
bool IsOpen() noexcept;

// Starts sleeping or waiting for aHours, as if the player confirmed the open menu.
void Confirm(float aHours) noexcept;
} // namespace SleepWaitMenu
