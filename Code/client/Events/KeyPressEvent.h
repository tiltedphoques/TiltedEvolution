#pragma once

/**
 * @brief Dispatched when VirtualKey is pressed.
 */
struct KeyPressEvent
{
    KeyPressEvent(const uint16_t acKeyCode) : VirtualKey(acKeyCode)
    {
    }

    uint16_t VirtualKey{};
};
