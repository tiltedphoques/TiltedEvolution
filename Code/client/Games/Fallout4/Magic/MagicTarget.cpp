#include <Magic/MagicTarget.h>
#include <Magic/EffectItem.h>
#include <Actor.h>
#include <PlayerCharacter.h>
#include <World.h>
#include <Events/AddTargetEvent.h>
#include <Events/HitEvent.h>
#include <Games/ActorExtension.h>
#include <Games/Overrides.h>
#include <Forms/SpellItem.h>
#include <Forms/EffectSetting.h>

TP_THIS_FUNCTION(TAddTarget, bool, MagicTarget, MagicTarget::AddTargetData& arData);
TP_THIS_FUNCTION(TCheckAddEffect, bool, MagicTarget::AddTargetData, void* apArgs, float afResistance);

static TAddTarget* RealAddTarget = nullptr;
static TCheckAddEffect* RealCheckAddEffect = nullptr;

static thread_local bool s_autoSucceedEffectCheck = false;

bool MagicTarget::AddTarget(AddTargetData& arData, bool, bool) noexcept
{
    s_autoSucceedEffectCheck = true;
    const bool result = TiltedPhoques::ThisCall(RealAddTarget, this, arData);
    s_autoSucceedEffectCheck = false;
    return result;
}

bool MagicTarget::AddTargetData::ShouldSync()
{
    // Constant slow time is an ability each client's scripts manage on their own player
    if (pEffectItem->IsSlowEffect())
    {
        SpellItem* pSpellItem = Cast<SpellItem>(pSpell);
        if (pSpellItem && pSpellItem->eCastingType == MagicSystem::CastingType::CONSTANT_EFFECT)
            return false;
    }

    return !pEffectItem->IsSummonEffect() && !pSpell->IsInvisibilitySpell() && !pSpell->IsWardSpell();
}

bool MagicTarget::AddTargetData::IsForbiddenEffect(Actor* apTarget)
{
    if (apTarget != PlayerCharacter::Get())
        return false;

    return pEffectItem->IsNightVisionEffect();
}

Actor* MagicTarget::GetTargetAsActor()
{
    TP_THIS_FUNCTION(TGetTargetAsActor, Actor*, MagicTarget);
    static VersionDbPtr<TGetTargetAsActor> getTargetAsActor(2226410);
    return TiltedPhoques::ThisCall(getTargetAsActor, this);
}

// Same ownership rules as Skyrim: effects are applied where the caster or the target is owned, then synced.
static bool TP_MAKE_THISCALL(HookAddTarget, MagicTarget, MagicTarget::AddTargetData& arData)
{
    Actor* pTargetActor = apThis->GetTargetAsActor();
    if (!pTargetActor || !arData.pSpell || !arData.pEffectItem || arData.IsForbiddenEffect(pTargetActor) || ScopedSpellCastOverride::IsOverriden())
        return TiltedPhoques::ThisCall(RealAddTarget, apThis, arData);

    ActorExtension* pTargetActorEx = pTargetActor->GetExtension();

    AddTargetEvent addTargetEvent{};
    addTargetEvent.TargetID = pTargetActor->formID;
    addTargetEvent.CasterID = arData.pCaster ? arData.pCaster->formID : 0;
    addTargetEvent.SpellID = arData.pSpell->formID;
    addTargetEvent.EffectID = arData.pEffectItem->pEffectSetting->formID;
    addTargetEvent.Magnitude = arData.fMagnitude;
    addTargetEvent.IsDualCasting = arData.bDualCast;

    if (pTargetActorEx->IsRemotePlayer())
    {
        if (!arData.pCaster || !arData.pCaster->GetExtension()->IsLocalPlayer())
            return false;
        if (!arData.pSpell->IsHealingSpell() && !arData.pSpell->IsBuffSpell())
            return false;

        const bool result = TiltedPhoques::ThisCall(RealAddTarget, apThis, arData);
        if (result)
            World::Get().GetRunner().Trigger(addTargetEvent);
        return result;
    }

    if (pTargetActorEx->IsLocalPlayer())
    {
        if (arData.pCaster && arData.pCaster->GetExtension()->IsRemotePlayer())
        {
            if (!World::Get().GetServerSettings().PvpEnabled)
                return false;
            // Heal and buff effects are synced by the caster
            if (arData.pSpell->IsHealingSpell() || arData.pSpell->IsBuffSpell())
                return false;
        }

        const bool result = TiltedPhoques::ThisCall(RealAddTarget, apThis, arData);
        if (result && arData.ShouldSync())
            World::Get().GetRunner().Trigger(addTargetEvent);
        return result;
    }

    if (arData.pCaster)
    {
        ActorExtension* pCasterExtension = arData.pCaster->GetExtension();
        if (pCasterExtension->IsLocalPlayer())
        {
            const bool result = TiltedPhoques::ThisCall(RealAddTarget, apThis, arData);
            if (result && arData.ShouldSync())
                World::Get().GetRunner().Trigger(addTargetEvent);
            return result;
        }

        if (pCasterExtension->IsRemotePlayer())
        {
            // The remote caster's client applies and syncs it, TakeDamage never runs here
            World::Get().GetRunner().Trigger(HitEvent(arData.pCaster->formID, pTargetActor->formID));
            return false;
        }
    }

    if (!pTargetActorEx->IsLocal())
        return false;

    const bool result = TiltedPhoques::ThisCall(RealAddTarget, apThis, arData);
    if (result && arData.ShouldSync())
        World::Get().GetRunner().Trigger(addTargetEvent);
    return result;
}

static bool TP_MAKE_THISCALL(HookCheckAddEffect, MagicTarget::AddTargetData, void* apArgs, float afResistance)
{
    if (s_autoSucceedEffectCheck)
        return true;

    return TiltedPhoques::ThisCall(RealCheckAddEffect, apThis, apArgs, afResistance);
}

static TiltedPhoques::Initializer s_magicTargetHooks(
    []()
    {
        // The base MagicTarget::AddTarget that Actor::AddTarget calls, and AddTargetData::CheckAddEffect
        static VersionDbPtr<TAddTarget> addTarget(2226407);
        static VersionDbPtr<TCheckAddEffect> checkAddEffect(2226406);

        RealAddTarget = addTarget.Get();
        RealCheckAddEffect = checkAddEffect.Get();

        TP_HOOK(&RealAddTarget, HookAddTarget);
        TP_HOOK(&RealCheckAddEffect, HookCheckAddEffect);
    });
