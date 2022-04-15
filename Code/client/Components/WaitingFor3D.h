#pragma once

#ifndef TP_INTERNAL_COMPONENTS_GUARD
#error Include Components.h instead
#endif

struct WaitingFor3D
{
    WaitingFor3D() = default;

    // This rather hacky thing is here because entt components seemingly can't be empty
    uint8_t Placeholder{};
};
