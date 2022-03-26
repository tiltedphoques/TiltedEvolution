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

TP_THIS_FUNCTION(TPickUpItem, char, PlayerCharacter, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, bool aUnk2);
TP_THIS_FUNCTION(TSetBeastForm, void, void, void* apUnk1, void* apUnk2, bool aEntering);
TP_THIS_FUNCTION(TAddSkillExperience, void, PlayerCharacter, int32_t aSkill, float aExperience);
TP_THIS_FUNCTION(TCalculateExperience, bool, int32_t, float* aFactor, float* aBonus, float* aUnk1, float* aUnk2);

static TPickUpItem* RealPickUpItem = nullptr;
static TSetBeastForm* RealSetBeastForm = nullptr;
static TAddSkillExperience* RealAddSkillExperience = nullptr;
static TCalculateExperience* RealCalculateExperience = nullptr;

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

extern thread_local bool g_modifyingInventory;

char TP_MAKE_THISCALL(HookPickUpItem, PlayerCharacter, TESObjectREFR* apObject, int32_t aCount, bool aUnk1, bool aUnk2)
{
    // TODO: this modifyingInventory check prolly isn't necessary here
    if (!g_modifyingInventory)
    {
        // This is here so that objects that are picked up on both clients, aka non temps, are synced through activation sync
        if (apObject->IsTemporary() && !ScopedActivateOverride::IsOverriden())
        {
            auto& modSystem = World::Get().GetModSystem();

            Inventory::Entry item{};
            modSystem.GetServerModId(apObject->baseForm->formID, item.BaseId);
            item.Count = aCount;

            // TODO: not sure about this
            if (apObject->GetExtraDataList())
                apThis->GetItemFromExtraData(item, apObject->GetExtraDataList());

            World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID, std::move(item)));
        }
    }

    return ThisCall(RealPickUpItem, apThis, apObject, aCount, aUnk1, aUnk2);
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
    POINTER_SKYRIMSE(TSetBeastForm, s_setBeastForm, 55497);
    POINTER_SKYRIMSE(TAddSkillExperience, s_addSkillExperience, 40488);
    POINTER_SKYRIMSE(TCalculateExperience, s_calculateExperience, 27244);

    RealPickUpItem = s_pickUpItem.Get();
    RealSetBeastForm = s_setBeastForm.Get();
    RealAddSkillExperience = s_addSkillExperience.Get();
    RealCalculateExperience = s_calculateExperience.Get();

    TP_HOOK(&RealPickUpItem, HookPickUpItem);
    TP_HOOK(&RealSetBeastForm, HookSetBeastForm);
    TP_HOOK(&RealAddSkillExperience, HookAddSkillExperience);
    TP_HOOK(&RealCalculateExperience, HookCalculateExperience);
});
