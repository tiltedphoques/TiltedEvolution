#include "PlayerCharacter.h"

#include <Games/References.h>
#include <Games/TES.h>
#include <Forms/TESObjectCELL.h>

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

void PlayerCharacter::PayCrimeGoldToAllFactions() noexcept
{
    // TODO: fallout 4 doesn't have "bounties".
    // Still, aggro should be removed somehow on respawn.
}

// TODO: ft (verify)
NiPoint3 PlayerCharacter::RespawnPlayer() noexcept
{
    // Make bleedout state recoverable
    SetNoBleedoutRecovery(false);

    DispelAllSpells();

    // Reset health to max
    // TODO(cosideci): there's a cleaner way to do this
    ForceActorValue(ActorValueOwner::ForceMode::DAMAGE, ActorValueInfo::kHealth, 1000000);

    TESObjectCELL* pCell = nullptr;

    if (GetWorldSpace())
    {
        // TP to Whiterun temple when killed in world space
        TES* pTes = TES::Get();
        pCell = ModManager::Get()->GetCellFromCoordinates(pTes->centerGridX, pTes->centerGridY, GetWorldSpace(), false);
    }
    else
    {
        // TP to start of cell when killed in an interior
        pCell = GetParentCell();
    }

    NiPoint3 pos{};
    NiPoint3 rot{};
    pCell->GetCOCPlacementInfo(&pos, &rot, true);

    MoveTo(pCell, pos);

    // Make bleedout state unrecoverable again for when the player goes down the next time
    SetNoBleedoutRecovery(true);

    return pos;
}
