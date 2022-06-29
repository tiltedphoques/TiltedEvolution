#include "PlayerCharacter.h"

#include <Games/References.h>

int32_t PlayerCharacter::LastUsedCombatSkill = -1;

void PlayerCharacter::SetDifficulty(const int32_t aDifficulty, bool aForceUpdate, bool aExpectGameDataLoaded) noexcept
{
    if (aDifficulty > 5 || aDifficulty < 0)
        return;

    TP_THIS_FUNCTION(TSetDifficulty, void, PlayerCharacter, const int32_t aDifficulty, bool aForceUpdate,
                     bool aExpectGameDataLoaded);
    POINTER_FALLOUT4(TSetDifficulty, setDifficulty, 657345);
    ThisCall(setDifficulty, this, aDifficulty, aForceUpdate, aExpectGameDataLoaded);
}

