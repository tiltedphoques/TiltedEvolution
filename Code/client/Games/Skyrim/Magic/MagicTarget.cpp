#include "MagicTarget.h"

#include <Actor.h>
#include <Games/ActorExtension.h>
#include <World.h>
#include <Games/ActorExtension.h>
#include "EffectItem.h"

#include <Structs/Skyrim/AnimationGraphDescriptor_WerewolfBehavior.h>
#include <Structs/Skyrim/AnimationGraphDescriptor_VampireLordBehavior.h>

#include <Events/AddTargetEvent.h>
#include <Events/HitEvent.h>

#include <Games/Overrides.h>
#include <PlayerCharacter.h>

#include <Effects/ActiveEffect.h>

#include <Events/RemoveSpellEvent.h>

TP_THIS_FUNCTION(TAddTarget, bool, MagicTarget, MagicTarget::AddTargetData& arData);
TP_THIS_FUNCTION(TRemoveSpell, bool, Actor, MagicItem* apSpell);
TP_THIS_FUNCTION(TCheckAddEffectTargetData, bool, MagicTarget::AddTargetData, void* arArgs, float afResistance);
TP_THIS_FUNCTION(TFindTargets, bool, MagicCaster, float afEffectivenessMult, int32_t* aruiTargetCount,
                 TESBoundObject* apSource, char abLoadCast, char abAdjust);
TP_THIS_FUNCTION(TAdjustForPerks, void, ActiveEffect, Actor* apCaster, MagicTarget* apTarget);
TP_THIS_FUNCTION(THasPerk, bool, Actor, TESForm* apPerk, void* apUnk1, double* afReturnValue);
TP_THIS_FUNCTION(TGetPerkRank, uint8_t, Actor, TESForm* apPerk);

static TAddTarget* RealAddTarget = nullptr;
static TRemoveSpell* RealRemoveSpell = nullptr;
static TCheckAddEffectTargetData* RealCheckAddEffectTargetData = nullptr;
static TFindTargets* RealFindTargets = nullptr;
static TAdjustForPerks* RealAdjustForPerks = nullptr;
static THasPerk* RealHasPerk = nullptr;
static TGetPerkRank* RealGetPerkRank = nullptr;

static thread_local bool s_autoSucceedEffectCheck = false;
static thread_local bool s_applyHealPerkBonus = false;
static thread_local bool s_applyStaminaPerkBonus = false;

void MagicTarget::DispelAllSpells(bool aNow) noexcept
{
    TP_THIS_FUNCTION(TDispelAllSpells, void, MagicTarget, bool aNow);
    POINTER_SKYRIMSE(TDispelAllSpells, dispelAllSpells, 34512);
    TiltedPhoques::ThisCall(dispelAllSpells, this, aNow);
}

bool MagicTarget::AddTarget(AddTargetData& arData, bool aApplyHealPerkBonus, bool aApplyStaminaPerkBonus) noexcept
{
    s_autoSucceedEffectCheck = true;
    s_applyHealPerkBonus = aApplyHealPerkBonus;
    s_applyStaminaPerkBonus = aApplyStaminaPerkBonus;

    bool result = TiltedPhoques::ThisCall(RealAddTarget, this, arData);

    s_autoSucceedEffectCheck = false;
    s_applyHealPerkBonus = false;
    s_applyStaminaPerkBonus = false;

    return result;
}

bool MagicTarget::AddTargetData::ShouldSync()
{
    return !pEffectItem->IsSummonEffect() && !pSpell->IsInvisibilitySpell() && !pSpell->IsWardSpell();
}

// Some effects are player specific, and do not need to be synced.
bool MagicTarget::AddTargetData::IsForbiddenEffect(Actor* apTarget)
{
    if (apTarget != PlayerCharacter::Get())
        return false;

    return pEffectItem->IsNightVisionEffect();
}

Actor* MagicTarget::GetTargetAsActor()
{
    TP_THIS_FUNCTION(TGetTargetAsActor, Actor*, MagicTarget);
    POINTER_SKYRIMSE(TGetTargetAsActor, getTargetAsActor, 34529);
    return TiltedPhoques::ThisCall(getTargetAsActor, this);
}

bool TP_MAKE_THISCALL(HookAddTarget, MagicTarget, MagicTarget::AddTargetData& arData)
{
    Actor* pTargetActor = apThis->GetTargetAsActor();
    if (!pTargetActor || arData.IsForbiddenEffect(pTargetActor))
        return TiltedPhoques::ThisCall(RealAddTarget, apThis, arData);

    ActorExtension* pTargetActorEx = pTargetActor->GetExtension();

    if (arData.pEffectItem->IsWerewolfEffect())
        pTargetActorEx->GraphDescriptorHash = AnimationGraphDescriptor_WerewolfBehavior::m_key;

    if (arData.pEffectItem->IsVampireLordEffect())
        pTargetActorEx->GraphDescriptorHash = AnimationGraphDescriptor_VampireLordBehavior::m_key;

    if (ScopedSpellCastOverride::IsOverriden())
        return TiltedPhoques::ThisCall(RealAddTarget, apThis, arData);

    AddTargetEvent addTargetEvent{};
    addTargetEvent.TargetID = pTargetActor->formID;
    addTargetEvent.CasterID = arData.pCaster ? arData.pCaster->formID : 0;
    addTargetEvent.SpellID = arData.pSpell->formID;
    addTargetEvent.EffectID = arData.pEffectItem->pEffectSetting->formID;
    addTargetEvent.Magnitude = arData.fMagnitude;
    addTargetEvent.IsDualCasting = arData.bDualCast;

    if (pTargetActorEx->IsRemotePlayer())
    {
        if (!arData.pCaster)
            return false;

        if (!arData.pSpell->IsHealingSpell() && !arData.pSpell->IsBuffSpell())
            return false;

        ActorExtension* pCasterExtension = arData.pCaster->GetExtension();
        if (!pCasterExtension->IsLocalPlayer())
            return false;

        if (arData.pSpell->IsHealingSpell())
        {
            addTargetEvent.ApplyHealPerkBonus = arData.pCaster->HasPerk(0x581f8);
            addTargetEvent.ApplyStaminaPerkBonus = arData.pCaster->HasPerk(0x581f9);
        }

        bool result = TiltedPhoques::ThisCall(RealAddTarget, apThis, arData);
        if (result)
            World::Get().GetRunner().Trigger(addTargetEvent);
        return result;
    }

    if (pTargetActorEx->IsLocalPlayer())
    {
        if (arData.pCaster)
        {
            ActorExtension* pCasterExtension = arData.pCaster->GetExtension();
            if (pCasterExtension->IsRemotePlayer())
            {
                if (!World::Get().GetServerSettings().PvpEnabled)
                    return false;

                // Heal and buff spells are already synced by the caster.
                if (arData.pSpell->IsHealingSpell() || arData.pSpell->IsBuffSpell())
                    return false;
            }
        }

        bool result = TiltedPhoques::ThisCall(RealAddTarget, apThis, arData);
        if (result && arData.ShouldSync())
            World::Get().GetRunner().Trigger(addTargetEvent);
        return result;
    }

    if (arData.pCaster)
    {
        ActorExtension* pCasterExtension = arData.pCaster->GetExtension();
        if (pCasterExtension->IsLocalPlayer())
        {
            bool result = TiltedPhoques::ThisCall(RealAddTarget, apThis, arData);
            if (result && arData.ShouldSync())
                World::Get().GetRunner().Trigger(addTargetEvent);
            return result;
        }
        else if (pCasterExtension->IsRemotePlayer())
        {
            // Send out a HitEvent because TakeDamage is never triggered.
            World::Get().GetRunner().Trigger(HitEvent(arData.pCaster->formID, pTargetActor->formID));
            return false;
        }
    }

    if (pTargetActorEx->IsLocal())
    {
        bool result = TiltedPhoques::ThisCall(RealAddTarget, apThis, arData);
        if (result && arData.ShouldSync())
            World::Get().GetRunner().Trigger(addTargetEvent);
        return result;
    }
    else
    {
        return false;
    }
}

// Designed to run when we hook removespell (AddressLib ID is 38717)
// Sends a message to the server to remove the spell from this player on other clients
bool TP_MAKE_THISCALL(HookRemoveSpell, Actor, MagicItem* apSpell)
{   
    bool result = TiltedPhoques::ThisCall(RealRemoveSpell, apThis, apSpell);
    if (apThis->GetExtension()->IsLocalPlayer() && result)
    {
        // Log spell info
        //spdlog::info("Removing spell {}, ID: {} from local player", apSpell->GetName() , apSpell->formID);
        RemoveSpellEvent removalEvent;

        removalEvent.TargetId = apThis->formID;
        removalEvent.SpellId = apSpell->formID;
        World::Get().GetRunner().Trigger(removalEvent);
    }

    return result;
}


bool TP_MAKE_THISCALL(HookCheckAddEffectTargetData, MagicTarget::AddTargetData, void* arArgs, float afResistance)
{
    if (s_autoSucceedEffectCheck)
        return true;

    return TiltedPhoques::ThisCall(RealCheckAddEffectTargetData, apThis, arArgs, afResistance);
}

bool TP_MAKE_THISCALL(HookFindTargets, MagicCaster, float afEffectivenessMult, int32_t* aruiTargetCount, TESBoundObject* apSource, char abLoadCast, char abAdjust)
{
    return TiltedPhoques::ThisCall(RealFindTargets, apThis, afEffectivenessMult, aruiTargetCount, apSource, abLoadCast, abAdjust);
}

void TP_MAKE_THISCALL(HookAdjustForPerks, ActiveEffect, Actor* apCaster, MagicTarget* apTarget)
{
    TiltedPhoques::ThisCall(RealAdjustForPerks, apThis, apCaster, apTarget);

    if (s_applyHealPerkBonus)
        apThis->fMagnitude *= 1.5f;
}

bool TP_MAKE_THISCALL(HookHasPerk, Actor, TESForm* apPerk, void* apUnk1, double* afReturnValue)
{
    if (apThis && apThis->GetExtension()->IsRemotePlayer())
    {
        if (apPerk && apPerk->formID == 0x581f9)
        {
            if (s_applyStaminaPerkBonus)
            {
                if (afReturnValue)
                    *afReturnValue = 1.f;
                return true;
            }
        }
    }

    return TiltedPhoques::ThisCall(RealHasPerk, apThis, apPerk, apUnk1, afReturnValue);
}

uint8_t TP_MAKE_THISCALL(HookGetPerkRank, Actor, TESForm* apPerk)
{
    return TiltedPhoques::ThisCall(RealGetPerkRank, apThis, apPerk);
}

static TiltedPhoques::Initializer s_magicTargetHooks([]() {
    POINTER_SKYRIMSE(TAddTarget, addTarget, 34526);
    POINTER_SKYRIMSE(TRemoveSpell, removeSpell, 38717);
    POINTER_SKYRIMSE(TCheckAddEffectTargetData, checkAddEffectTargetData, 34525);
    POINTER_SKYRIMSE(TFindTargets, findTargets, 34410);
    POINTER_SKYRIMSE(TAdjustForPerks, adjustForPerks, 34053);
    POINTER_SKYRIMSE(THasPerk, hasPerk, 21622);
    POINTER_SKYRIMSE(TGetPerkRank, getPerkRank, 37698);

    RealAddTarget = addTarget.Get();
    RealRemoveSpell = removeSpell.Get();
    RealCheckAddEffectTargetData = checkAddEffectTargetData.Get();
    RealFindTargets = findTargets.Get();
    RealAdjustForPerks = adjustForPerks.Get();
    RealHasPerk = hasPerk.Get();
    RealGetPerkRank = getPerkRank.Get();

    TP_HOOK(&RealAddTarget, HookAddTarget);
    TP_HOOK(&RealRemoveSpell, HookRemoveSpell);
    TP_HOOK(&RealCheckAddEffectTargetData, HookCheckAddEffectTargetData);
    TP_HOOK(&RealFindTargets, HookFindTargets);
    TP_HOOK(&RealAdjustForPerks, HookAdjustForPerks);
    TP_HOOK(&RealHasPerk, HookHasPerk);
    //TP_HOOK(&RealGetPerkRank, HookGetPerkRank);
});
