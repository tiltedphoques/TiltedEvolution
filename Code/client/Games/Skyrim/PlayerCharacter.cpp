#include <PlayerCharacter.h>

#include <Games/Overrides.h>

#include <Events/InventoryChangeEvent.h>
#include <Events/AddExperienceEvent.h>

#include <World.h>

#include <Games/Skyrim/Forms/ActorValueInfo.h>
#include <Games/ActorExtension.h>

TP_THIS_FUNCTION(TPickUpItem, char, PlayerCharacter, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, bool aUnk2);
TP_THIS_FUNCTION(TAddSkillExperience, void, PlayerCharacter, int32_t aSkill, float aExperience);
TP_THIS_FUNCTION(TCalculateExperience, bool, int32_t, float* aFactor, float* aBonus, float* aUnk1, float* aUnk2);

static TPickUpItem* RealPickUpItem = nullptr;
static TAddSkillExperience* RealAddSkillExperience = nullptr;
static TCalculateExperience* RealCalculateExperience = nullptr;

// TODO: scoped override

void PlayerCharacter::AddSkillExperience(int32_t aSkill, float aExperience) noexcept
{
    Skills::Skill skill = Skills::GetSkillFromActorValue(aSkill);
    float oldExperience = GetSkillExperience(skill);

    ScopedExperienceOverride _;

    ThisCall(RealAddSkillExperience, this, aSkill, aExperience);

    float newExperience = GetSkillExperience(skill);
    float deltaExperience = newExperience - oldExperience;

    spdlog::info("Added {} experience to skill {}", deltaExperience, aSkill);
}

char TP_MAKE_THISCALL(HookPickUpItem, PlayerCharacter, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, bool aUnk2)
{
    World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID));
    return ThisCall(RealPickUpItem, apThis, apObject, aCount, aUnk1, aUnk2);
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

    spdlog::info("Skill (AVI): {}, experience: {}", aSkill, deltaExperience);

    if (combatSkills.contains(aSkill))
    {
        spdlog::info("Set new last used combat skill to {}.", aSkill);
        apThis->GetExtension()->LastUsedCombatSkill = aSkill;

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
    POINTER_SKYRIMSE(TPickUpItem, s_pickUpItem, 40533);
    POINTER_SKYRIMSE(TAddSkillExperience, s_addSkillExperience, 40488);
    POINTER_SKYRIMSE(TCalculateExperience, s_calculateExperience, 27244);

    RealPickUpItem = s_pickUpItem.Get();
    RealAddSkillExperience = s_addSkillExperience.Get();
    RealCalculateExperience = s_calculateExperience.Get();

    TP_HOOK(&RealPickUpItem, HookPickUpItem);
    TP_HOOK(&RealAddSkillExperience, HookAddSkillExperience);
    TP_HOOK(&RealCalculateExperience, HookCalculateExperience);
});
