#include <TiltedOnlinePCH.h>

#include <PlayerCharacter.h>

#include <Events/InventoryChangeEvent.h>

#include <World.h>

#include <Games/Skyrim/Forms/ActorValueInfo.h>
#include <Games/ActorExtension.h>

TP_THIS_FUNCTION(TPickUpItem, char, PlayerCharacter, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, bool aUnk2);
TP_THIS_FUNCTION(TAddSkillExperience, void, PlayerCharacter, int32_t aSkill, float aExperience);

static TPickUpItem* RealPickUpItem = nullptr;
static TAddSkillExperience* RealAddSkillExperience = nullptr;

void PlayerCharacter::AddSkillExperience(int32_t aSkill, float aExperience) noexcept
{
    ThisCall(RealAddSkillExperience, this, aSkill, aExperience);
}

char TP_MAKE_THISCALL(HookPickUpItem, PlayerCharacter, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, bool aUnk2)
{
    World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID));
    return ThisCall(RealPickUpItem, apThis, apObject, aCount, aUnk1, aUnk2);
}

void TP_MAKE_THISCALL(HookAddSkillExperience, PlayerCharacter, int32_t aSkill, float aExperience)
{
    // TODO: armor skills?
    static const Set<int32_t> combatSkills{ActorValueInfo::kAlteration, ActorValueInfo::kConjuration, ActorValueInfo::kDestruction,
                                 ActorValueInfo::kIllusion, ActorValueInfo::kRestoration, ActorValueInfo::kOneHanded,
                                 ActorValueInfo::kTwoHanded, ActorValueInfo::kMarksman, ActorValueInfo::kBlock};

    if (combatSkills.contains(aSkill))
    {
        spdlog::info("Set new last used combat skill.");
        apThis->GetExtension()->LastUsedCombatSkill = aSkill;
    }

    // TODO: send event

    spdlog::info("Skill: {}, experience: {}", aSkill, aExperience);
    ThisCall(RealAddSkillExperience, apThis, aSkill, aExperience);
}

static TiltedPhoques::Initializer s_playerCharacterHooks([]()
{
    POINTER_SKYRIMSE(TPickUpItem, s_pickUpItem, 40533);
    POINTER_SKYRIMSE(TAddSkillExperience, s_addSkillExperience, 40488);

    RealPickUpItem = s_pickUpItem.Get();
    RealAddSkillExperience = s_addSkillExperience.Get();

    TP_HOOK(&RealPickUpItem, HookPickUpItem);
    TP_HOOK(&RealAddSkillExperience, HookAddSkillExperience);
});
