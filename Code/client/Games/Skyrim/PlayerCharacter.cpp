#include <PlayerCharacter.h>
#include <Games/ActorExtension.h>

#include <Structs/Skyrim/AnimationGraphDescriptor_Master_Behavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_VampireLordBehavior.h>

#include <Games/Overrides.h>

#include <Events/InventoryChangeEvent.h>
#include <Events/BeastFormChangeEvent.h>
#include <Events/AddExperienceEvent.h>
#include <Events/SetWaypointEvent.h>
#include <Events/RemoveWaypointEvent.h>

#include <World.h>

#include <Games/Skyrim/Forms/ActorValueInfo.h>
#include <Games/ActorExtension.h>
#include <Games/TES.h>
#include <Games/References.h>
#include <Forms/TESWorldSpace.h>

#include <Forms/TESObjectCELL.h>

int32_t PlayerCharacter::LastUsedCombatSkill = -1;

TP_THIS_FUNCTION(TPickUpObject, char, PlayerCharacter, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, bool aUnk2);
TP_THIS_FUNCTION(TSetBeastForm, void, void, void* apUnk1, void* apUnk2, bool aEntering);
TP_THIS_FUNCTION(TAddSkillExperience, void, PlayerCharacter, int32_t aSkill, float aExperience);
TP_THIS_FUNCTION(TCalculateExperience, bool, int32_t, float* aFactor, float* aBonus, float* aUnk1, float* aUnk2);
TP_THIS_FUNCTION(TSetWaypoint, void, PlayerCharacter, NiPoint3* apPosition, TESWorldSpace* apWorldSpace);
TP_THIS_FUNCTION(TRemoveWaypoint, void, PlayerCharacter);

static TPickUpObject* RealPickUpObject = nullptr;
static TSetBeastForm* RealSetBeastForm = nullptr;
static TAddSkillExperience* RealAddSkillExperience = nullptr;
static TCalculateExperience* RealCalculateExperience = nullptr;
static TSetWaypoint* RealSetWaypoint = nullptr;
static TRemoveWaypoint* RealRemoveWaypoint = nullptr;

void PlayerCharacter::SetGodMode(bool aSet) noexcept
{
    POINTER_SKYRIMSE(bool, bGodMode, 404238);
    *bGodMode.Get() = aSet;
}

void PlayerCharacter::SetDifficulty(const int32_t aDifficulty, bool aForceUpdate, bool aExpectGameDataLoaded) noexcept
{
    if (aDifficulty > 5 || aDifficulty < 0)
        return;

    int32_t* difficultySetting = Settings::GetDifficulty();
    *difficultySetting = difficulty = aDifficulty;
}

void PlayerCharacter::AddSkillExperience(int32_t aSkill, float aExperience) noexcept
{
    Skills::Skill skill = Skills::GetSkillFromActorValue(aSkill);
    float oldExperience = GetSkillExperience(skill);

    ScopedExperienceOverride _;

    TiltedPhoques::ThisCall(RealAddSkillExperience, this, aSkill, aExperience);

    float newExperience = GetSkillExperience(skill);
    float deltaExperience = newExperience - oldExperience;

    spdlog::debug("Added {} experience to skill {}", deltaExperience, aSkill);
}

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

void PlayerCharacter::PayCrimeGoldToAllFactions() noexcept
{
    // Yes, yes, this isn't great, but there's no "pay fines everywhere" function
    const uint32_t crimeFactionIds[]{0x28170, 0x267E3, 0x29DB0, 0x2816D, 0x2816e, 0x2816C, 0x2816B, 0x267EA, 0x2816F, 0x4018279};

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
}

void PlayerCharacter::SetWaypoint(NiPoint3* apPosition, TESWorldSpace* apWorldSpace) noexcept
{
    return TiltedPhoques::ThisCall(RealSetWaypoint, this, apPosition, apWorldSpace);
}

void PlayerCharacter::RemoveWaypoint() noexcept
{
    return TiltedPhoques::ThisCall(RealRemoveWaypoint, this);
}

char TP_MAKE_THISCALL(HookPickUpObject, PlayerCharacter, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, bool aUnk2)
{
    auto& modSystem = World::Get().GetModSystem();

    Inventory::Entry item{};
    modSystem.GetServerModId(apObject->baseForm->formID, item.BaseId);
    item.Count = aCount;

    if (apObject->GetExtraDataList() && !ScopedExtraDataOverride::IsOverriden())
    {
        ScopedExtraDataOverride _;
        apThis->GetItemFromExtraData(item, apObject->GetExtraDataList());
    }

    // This is here so that objects that are picked up on both clients, aka non temps, are synced through activation sync.
    // The inventory change event should always be sent to the server, otherwise the server inventory won't be updated.
    bool shouldUpdateClients = apObject->IsTemporary() && !ScopedActivateOverride::IsOverriden();

    World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID, std::move(item), false, shouldUpdateClients));

    ScopedInventoryOverride _;

    return TiltedPhoques::ThisCall(RealPickUpObject, apThis, apObject, aCount, aUnk1, aUnk2);
}

void TP_MAKE_THISCALL(HookSetBeastForm, void, void* apUnk1, void* apUnk2, bool aEntering)
{
    if (!aEntering)
    {
        PlayerCharacter::Get()->GetExtension()->GraphDescriptorHash = AnimationGraphDescriptor_Master_Behavior::m_key;
#ifdef MODDED_BEHAVIOR_COMPATIBILITY
        // Restore instead to saved, modified behavior hash. IF Nemesis/Pandora is installed, otherwise BehaviorVar::Patch() doesn't run.
        auto hash = PlayerCharacter::Get()->GetExtension()->HumanoidGraphDescriptorHash;
        if (hash)
            PlayerCharacter::Get()->GetExtension()->GraphDescriptorHash = hash;
#endif MODDED_BEHAVIOR_COMPATIBILITY

        World::Get().GetRunner().Trigger(BeastFormChangeEvent());
    }

    TiltedPhoques::ThisCall(RealSetBeastForm, apThis, apUnk1, apUnk2, aEntering);
}

void TP_MAKE_THISCALL(HookAddSkillExperience, PlayerCharacter, int32_t aSkill, float aExperience)
{
    // TODO: armor skills? sneak?
    static const Set<int32_t> combatSkills{ActorValueInfo::kAlteration, ActorValueInfo::kConjuration, ActorValueInfo::kDestruction, ActorValueInfo::kIllusion, ActorValueInfo::kRestoration, ActorValueInfo::kOneHanded, ActorValueInfo::kTwoHanded, ActorValueInfo::kMarksman, ActorValueInfo::kBlock};

    Skills::Skill skill = Skills::GetSkillFromActorValue(aSkill);
    float oldExperience = apThis->GetSkillExperience(skill);

    TiltedPhoques::ThisCall(RealAddSkillExperience, apThis, aSkill, aExperience);

    float newExperience = apThis->GetSkillExperience(skill);
    float deltaExperience = newExperience - oldExperience;

    spdlog::debug("Skill (AVI): {}, experience: {}", aSkill, deltaExperience);

    if (combatSkills.contains(aSkill))
    {
        spdlog::debug("Set new last used combat skill to {}.", aSkill);
        PlayerCharacter::LastUsedCombatSkill = aSkill;

        World::Get().GetRunner().Trigger(AddExperienceEvent(deltaExperience));
    }
}

bool TP_MAKE_THISCALL(HookCalculateExperience, int32_t, float* aFactor, float* aBonus, float* aUnk1, float* aUnk2)
{
    bool result = TiltedPhoques::ThisCall(RealCalculateExperience, apThis, aFactor, aBonus, aUnk1, aUnk2);

    if (ScopedExperienceOverride::IsOverriden())
    {
        *aFactor = 1.f;
        *aBonus = 0.f;
    }

    return result;
}

void TP_MAKE_THISCALL(HookSetWaypoint, PlayerCharacter, NiPoint3* apPosition, TESWorldSpace* apWorldSpace)
{
    Vector3_NetQuantize position{};
    position.x = apPosition->x;
    position.y = apPosition->y;
    position.z = apPosition->z;

    World::Get().GetRunner().Trigger(SetWaypointEvent(position, apWorldSpace ? apWorldSpace->formID : 0));

    return TiltedPhoques::ThisCall(RealSetWaypoint, apThis, apPosition, apWorldSpace);
}

void TP_MAKE_THISCALL(HookRemoveWaypoint, PlayerCharacter)
{
    World::Get().GetRunner().Trigger(RemoveWaypointEvent());

    return TiltedPhoques::ThisCall(RealRemoveWaypoint, apThis);
}

static TiltedPhoques::Initializer s_playerCharacterHooks(
    []()
    {
        POINTER_SKYRIMSE(TPickUpObject, s_pickUpObject, 40533);
        POINTER_SKYRIMSE(TSetBeastForm, s_setBeastForm, 55497);
        POINTER_SKYRIMSE(TAddSkillExperience, s_addSkillExperience, 40488);
        POINTER_SKYRIMSE(TCalculateExperience, s_calculateExperience, 27244);
        POINTER_SKYRIMSE(TSetWaypoint, s_setWaypoint, 40535);
        POINTER_SKYRIMSE(TRemoveWaypoint, s_removeWaypoint, 40536);

        RealPickUpObject = s_pickUpObject.Get();
        RealSetBeastForm = s_setBeastForm.Get();
        RealAddSkillExperience = s_addSkillExperience.Get();
        RealCalculateExperience = s_calculateExperience.Get();
        RealSetWaypoint = s_setWaypoint.Get();
        RealRemoveWaypoint = s_removeWaypoint.Get();

        TP_HOOK(&RealPickUpObject, HookPickUpObject);
        TP_HOOK(&RealSetBeastForm, HookSetBeastForm);
        TP_HOOK(&RealAddSkillExperience, HookAddSkillExperience);
        TP_HOOK(&RealCalculateExperience, HookCalculateExperience);
        TP_HOOK(&RealSetWaypoint, HookSetWaypoint);
        TP_HOOK(&RealRemoveWaypoint, HookRemoveWaypoint);
    });
