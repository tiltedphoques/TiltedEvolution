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

// TODO: ft
void PlayerCharacter::PayCrimeGoldToAllFactions() noexcept
{
    /*
    // Yes, yes, this isn't great, but there's no "pay fines everywhere" function
    TiltedPhoques::Vector<uint32_t> crimeFactionIds{ 0x28170, 0x267E3, 0x29DB0, 0x2816D, 0x2816e, 0x2816C, 0x2816B, 0x267EA, 0x2816F, 0x4018279 };

    for (uint32_t crimeFactionId : crimeFactionIds)
    {
        TESFaction* pCrimeFaction = Cast<TESFaction>(TESForm::GetById(crimeFactionId));
        if (!pCrimeFaction)
        {
            spdlog::error("This isn't a crime faction! {:X}", crimeFactionId);
            continue;
        }

        PayFine(pCrimeFaction, false, false);
    }
    */
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
