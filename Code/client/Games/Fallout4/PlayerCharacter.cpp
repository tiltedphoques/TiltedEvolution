#include <TiltedOnlinePCH.h>

#include <PlayerCharacter.h>

int32_t PlayerCharacter::LastUsedCombatSkill = -1;

PlayerCharacter* PlayerCharacter::Get() noexcept
{
    static VersionDbPtr<PlayerCharacter*> s_character(4798212);
    return *s_character.Get();
}

int32_t PlayerCharacter::GetDifficulty() noexcept
{
    TP_THIS_FUNCTION(TGetDifficulty, int32_t, PlayerCharacter);
    static VersionDbPtr<TGetDifficulty> getDifficulty(2233056);
    return TiltedPhoques::ThisCall(getDifficulty, this);
}

void PlayerCharacter::SetDifficulty(int32_t aDifficulty, bool aForceUpdate, bool aExpectGameDataLoaded) noexcept
{
    if (aDifficulty < 0 || aDifficulty > 6)
        return;

    TP_THIS_FUNCTION(TSetDifficulty, void, PlayerCharacter, int32_t, bool, bool);
    static VersionDbPtr<TSetDifficulty> setDifficulty(2233057);
    TiltedPhoques::ThisCall(setDifficulty, this, aDifficulty, aForceUpdate, aExpectGameDataLoaded);
}

void PlayerCharacter::SetGodMode(bool aSet) noexcept
{
    using TSetGodMode = void(bool);
    static VersionDbPtr<TSetGodMode> setGodMode(2232985);
    setGodMode.Get()(aSet);
}
