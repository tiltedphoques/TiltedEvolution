#pragma once

struct PlayerCamera
{
    static PlayerCamera* Get() noexcept;

    bool IsFirstPerson() noexcept;
};
