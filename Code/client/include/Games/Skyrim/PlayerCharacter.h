#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Actor.h>

struct PlayerCharacter : Actor
{
    static constexpr uint32_t Type = FormType::Character;

    static PlayerCharacter* Get() noexcept;

    uint8_t padPlayerEnd[0xBE0 - sizeof(Actor)];
};

static_assert(sizeof(PlayerCharacter) == 0xBE0);

#endif
