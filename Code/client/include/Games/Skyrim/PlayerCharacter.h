#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Actor.h>
#include <Games/Skyrim/Misc/TintMask.h>

struct PlayerCharacter : Actor
{
    static constexpr uint32_t Type = FormType::Character;

    static PlayerCharacter* Get() noexcept;

    const GameArray<TintMask*>& GetTints() const noexcept;

    uint8_t pad1[0xAC8 - sizeof(Actor)];
    TESForm *locationForm;
    uint8_t padAC8[0x40];

    GameArray<TintMask*> baseTints;
    GameArray<TintMask*>* overlayTints;

    uint8_t padPlayerEnd[0xBE0 - 0xB30];
};

static_assert(sizeof(PlayerCharacter) == 0xBE0);
static_assert(offsetof(PlayerCharacter, locationForm) == 0xAC8);
static_assert(offsetof(PlayerCharacter, baseTints) == 0xB10);
static_assert(offsetof(PlayerCharacter, overlayTints) == 0xB28);
#endif
