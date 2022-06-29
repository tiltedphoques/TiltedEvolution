#pragma once

struct ActorState
{
    virtual ~ActorState();

    uint32_t flags1;
    uint32_t flags2;

    bool IsWeaponDrawn() const noexcept
    {
        return ((flags2 >> 1) & 7) >= 3;
    }
    // TODO: ft, does it need fully drawn check?
    bool IsWeaponFullyDrawn() const noexcept
    {
        return IsWeaponDrawn();
    }

    // TODO: ft (verify)
    bool IsBleedingOut() const noexcept
    {
        return ((flags1 >> 17) & 0xFu) - 7 <= 1;
    }

    bool SetWeaponDrawn(bool aDraw) noexcept;
};
