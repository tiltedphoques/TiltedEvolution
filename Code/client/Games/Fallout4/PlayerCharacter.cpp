#include <TiltedOnlinePCH.h>

#include <PlayerCharacter.h>

#include <Forms/ActorValueInfo.h>
#include <Forms/TESObjectCELL.h>
#include <Games/TES.h>
#include <Events/SetWaypointEvent.h>
#include <Events/RemoveWaypointEvent.h>
#include <World.h>

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

void PlayerCharacter::AddSkillExperience(int32_t, float) noexcept
{
    // Fallout 4 has no skills; shared party experience is not mapped yet.
}

NiPoint3 PlayerCharacter::RespawnPlayer() noexcept
{
    // Make bleedout state recoverable
    SetNoBleedoutRecovery(false);

    ForceActorValue(ActorValueOwner::ForceMode::DAMAGE, ActorValueInfo::kHealth, 1000000);

    // An essential player goes "essential down" instead of bleeding out and only gets up through this.
    TP_THIS_FUNCTION(TSetEssentialDown, void, Actor, bool);
    static VersionDbPtr<TSetEssentialDown> setEssentialDown(2230033);
    TiltedPhoques::ThisCall(setEssentialDown, static_cast<Actor*>(this), false);

    TESObjectCELL* pCell = nullptr;
    if (auto* pWorldSpace = GetWorldSpace())
    {
        TES* pTes = TES::Get();
        pCell = ModManager::Get()->GetCellFromCoordinates(pTes->centerGridX, pTes->centerGridY, pWorldSpace, false);
    }
    else
    {
        pCell = GetSaveParentCell();
    }

    NiPoint3 pos{};
    NiPoint3 rot{};

    if (pCell)
    {
        pCell->GetCOCPlacementInfo(&pos, &rot, true);
        MoveTo(pCell, pos);
    }

    // Make bleedout state unrecoverable again for when the player goes down the next time
    SetNoBleedoutRecovery(true);

    return pos;
}

void PlayerCharacter::PayCrimeGoldToAllFactions() noexcept
{
    // Fallout 4 crime factions differ from Skyrim's holds; not implemented yet.
}

TP_THIS_FUNCTION(TSetPlayerMapMarker, void, PlayerCharacter, const NiPoint3* apPosition, TESForm* apSpace);
TP_THIS_FUNCTION(TRemovePlayerMapMarker, void, PlayerCharacter);

static TSetPlayerMapMarker* RealSetPlayerMapMarker = nullptr;
static TRemovePlayerMapMarker* RealRemovePlayerMapMarker = nullptr;

void PlayerCharacter::SetWaypoint(NiPoint3* apPosition, TESWorldSpace* apWorldSpace) noexcept
{
    TiltedPhoques::ThisCall(RealSetPlayerMapMarker, this, apPosition, reinterpret_cast<TESForm*>(apWorldSpace));
}

void PlayerCharacter::RemoveWaypoint() noexcept
{
    TiltedPhoques::ThisCall(RealRemovePlayerMapMarker, this);
}

void TP_MAKE_THISCALL(HookSetPlayerMapMarker, PlayerCharacter, const NiPoint3* apPosition, TESForm* apSpace)
{
    Vector3_NetQuantize position{};
    position.x = apPosition->x;
    position.y = apPosition->y;
    position.z = apPosition->z;

    World::Get().GetRunner().Trigger(SetWaypointEvent(position, apSpace ? apSpace->formID : 0));

    TiltedPhoques::ThisCall(RealSetPlayerMapMarker, apThis, apPosition, apSpace);
}

void TP_MAKE_THISCALL(HookRemovePlayerMapMarker, PlayerCharacter)
{
    World::Get().GetRunner().Trigger(RemoveWaypointEvent());

    TiltedPhoques::ThisCall(RealRemovePlayerMapMarker, apThis);
}

static TiltedPhoques::Initializer s_playerCharacterHooks(
    []()
    {
        static VersionDbPtr<TSetPlayerMapMarker> setPlayerMapMarker(2233021);
        static VersionDbPtr<TRemovePlayerMapMarker> removePlayerMapMarker(2233022);

        RealSetPlayerMapMarker = setPlayerMapMarker.Get();
        RealRemovePlayerMapMarker = removePlayerMapMarker.Get();

        TP_HOOK(&RealSetPlayerMapMarker, HookSetPlayerMapMarker);
        TP_HOOK(&RealRemovePlayerMapMarker, HookRemovePlayerMapMarker);
    });
