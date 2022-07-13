#include <PlayerCharacter.h>
#include <Games/ActorExtension.h>

#include <Structs/Skyrim/AnimationGraphDescriptor_Master_Behavior.h>

#include <Games/Overrides.h>

#include <Events/InventoryChangeEvent.h>
#include <Events/LeaveBeastFormEvent.h>
#include <Events/AddExperienceEvent.h>

#include <World.h>

#include <Games/Skyrim/Forms/ActorValueInfo.h>
#include <Games/ActorExtension.h>
#include <Games/TES.h>
#include <Games/References.h>

#include <Forms/TESObjectCELL.h>

int32_t PlayerCharacter::LastUsedCombatSkill = -1;

TP_THIS_FUNCTION(TPickUpObject, char, PlayerCharacter, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, bool aUnk2);
TP_THIS_FUNCTION(TSetBeastForm, void, void, void* apUnk1, void* apUnk2, bool aEntering);
TP_THIS_FUNCTION(TAddSkillExperience, void, PlayerCharacter, int32_t aSkill, float aExperience);
TP_THIS_FUNCTION(TCalculateExperience, bool, int32_t, float* aFactor, float* aBonus, float* aUnk1, float* aUnk2);

static TPickUpObject* RealPickUpObject = nullptr;
static TSetBeastForm* RealSetBeastForm = nullptr;
static TAddSkillExperience* RealAddSkillExperience = nullptr;
static TCalculateExperience* RealCalculateExperience = nullptr;

void PlayerCharacter::SetGodMode(bool aSet) noexcept
{
    POINTER_SKYRIMSE(bool, bGodMode, 404238);
    *bGodMode.Get() = aSet;
}

void PlayerCharacter::SetDifficulty(const int32_t aDifficulty) noexcept
{
    if (aDifficulty > 5)
        return;

    int32_t* difficultySetting = Settings::GetDifficulty();
    *difficultySetting = difficulty = aDifficulty;
}

void PlayerCharacter::AddSkillExperience(int32_t aSkill, float aExperience) noexcept
{
    Skills::Skill skill = Skills::GetSkillFromActorValue(aSkill);
    float oldExperience = GetSkillExperience(skill);

    ScopedExperienceOverride _;

    ThisCall(RealAddSkillExperience, this, aSkill, aExperience);

    float newExperience = GetSkillExperience(skill);
    float deltaExperience = newExperience - oldExperience;

    spdlog::debug("Added {} experience to skill {}", deltaExperience, aSkill);
}

NiPoint3 PlayerCharacter::RespawnPlayer() noexcept
{
    // Make bleedout state recoverable
    SetNoBleedoutRecovery(false);

    DispellAllSpells();

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
}

char TP_MAKE_THISCALL(HookPickUpObject, PlayerCharacter, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, bool aUnk2)
{
    // This is here so that objects that are picked up on both clients, aka non temps, are synced through activation sync
    if (apObject->IsTemporary() && !ScopedActivateOverride::IsOverriden())
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
        World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID, std::move(item)));
    }

    ScopedInventoryOverride _;

    return ThisCall(RealPickUpObject, apThis, apObject, aCount, aUnk1, aUnk2);
}

void TP_MAKE_THISCALL(HookSetBeastForm, void, void* apUnk1, void* apUnk2, bool aEntering)
{
    if (!aEntering)
    {
        PlayerCharacter::Get()->GetExtension()->GraphDescriptorHash = AnimationGraphDescriptor_Master_Behavior::m_key;
        World::Get().GetRunner().Trigger(LeaveBeastFormEvent());
    }

    ThisCall(RealSetBeastForm, apThis, apUnk1, apUnk2, aEntering);
}

void TP_MAKE_THISCALL(HookAddSkillExperience, PlayerCharacter, int32_t aSkill, float aExperience)
{
    // TODO: armor skills? sneak?
    static const Set<int32_t> combatSkills{ActorValueInfo::kAlteration, ActorValueInfo::kConjuration, ActorValueInfo::kDestruction,
                                           ActorValueInfo::kIllusion, ActorValueInfo::kRestoration, ActorValueInfo::kOneHanded,
                                           ActorValueInfo::kTwoHanded, ActorValueInfo::kMarksman, ActorValueInfo::kBlock};

    Skills::Skill skill = Skills::GetSkillFromActorValue(aSkill);
    float oldExperience = apThis->GetSkillExperience(skill);

    ThisCall(RealAddSkillExperience, apThis, aSkill, aExperience);

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
    bool result = ThisCall(RealCalculateExperience, apThis, aFactor, aBonus, aUnk1, aUnk2);

    if (ScopedExperienceOverride::IsOverriden())
    {
        *aFactor = 1.f;
        *aBonus = 0.f;
    }

    return result;
}

static TiltedPhoques::Initializer s_playerCharacterHooks([]()
{
    POINTER_SKYRIMSE(TPickUpObject, s_pickUpObject, 40533);
    POINTER_SKYRIMSE(TSetBeastForm, s_setBeastForm, 55497);
    POINTER_SKYRIMSE(TAddSkillExperience, s_addSkillExperience, 40488);
    POINTER_SKYRIMSE(TCalculateExperience, s_calculateExperience, 27244);

    RealPickUpObject = s_pickUpObject.Get();
    RealSetBeastForm = s_setBeastForm.Get();
    RealAddSkillExperience = s_addSkillExperience.Get();
    RealCalculateExperience = s_calculateExperience.Get();

    TP_HOOK(&RealPickUpObject, HookPickUpObject);
    TP_HOOK(&RealSetBeastForm, HookSetBeastForm);
    TP_HOOK(&RealAddSkillExperience, HookAddSkillExperience);
    TP_HOOK(&RealCalculateExperience, HookCalculateExperience);
});
