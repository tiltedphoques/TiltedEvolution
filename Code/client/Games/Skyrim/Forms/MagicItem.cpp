#include "MagicItem.h"

bool MagicItem::IsWardSpell() const noexcept
{
    BGSKeyword* pMagicWard = Cast<BGSKeyword>(TESForm::GetById(0x1ea69));

    if (keyword.count > 0 && keyword.Contains(pMagicWard))
        return true;

    // Spells typically don't have keywords, so we must check their effects
    for (const EffectItem* pEffect : listOfEffects)
    {
        if (pEffect->pEffectSetting && pEffect->pEffectSetting->keywordForm.count > 0 && 
            pEffect->pEffectSetting->keywordForm.Contains(pMagicWard))
            return true;
    }

    return false;
}

bool MagicItem::IsInvisibilitySpell() const noexcept
{
    // PR #796 and the "SkyPatcher Keyword Framework" mod both hoist
    // some keywords from the effects up to the spell, because some 
    // spell records are buggy; this fixes things like syncing more ward spells.
    // But note sometimes that breaks other things. This is the first one found,
    // Bow of Shadows Invisibility (formID 0xFE003805), which doesn't have an 
    // Invisibility keyword. Apparently deliberately, it isn't really a spell but a 
    // condition (readying the bow) that triggers an effect. If it is treated as a 
    // spell, syncing doesn't work. There may be others, if so, there will be a 
    // more general fix.
    if (formID == 0xfe003805)
        return false;

    BGSKeyword* pMagicInvisibility = Cast<BGSKeyword>(TESForm::GetById(0x1ea6f));

    if (keyword.count > 0 && keyword.Contains(pMagicInvisibility))
        return true;

    for (const EffectItem* pEffect : listOfEffects)
    {
        if (pEffect->pEffectSetting && pEffect->pEffectSetting->keywordForm.count > 0 &&
            pEffect->pEffectSetting->keywordForm.Contains(pMagicInvisibility))
            return true;
    }
    return false;
}

bool MagicItem::IsHealingSpell() const noexcept
{
    BGSKeyword* pMagicRestoreHealth = Cast<BGSKeyword>(TESForm::GetById(0x1ceb0));
    
    if (keyword.count > 0 && keyword.Contains(pMagicRestoreHealth))
        return true;

    for (const EffectItem* pEffect : listOfEffects)
    {
        if (pEffect->pEffectSetting && pEffect->pEffectSetting->keywordForm.count > 0 &&
            pEffect->pEffectSetting->keywordForm.Contains(pMagicRestoreHealth))
            return true;
    }
    return false;
}

bool MagicItem::IsBuffSpell() const noexcept
{
    switch (formID)
    {
    case 0x4dee8: // Courage
    case 0x4deec: // Rally
    case 0x7e8dd: // Call to Arms
        return true;
    default:
        return false;
    }
}

bool MagicItem::IsBoundWeaponSpell() noexcept
{
    for (EffectItem* pEffect : listOfEffects)
    {
        if (pEffect->pEffectSetting && pEffect->pEffectSetting->eArchetype == EffectArchetypes::ArchetypeID::kBoundWeapon)
            return true;
    }

    return false;
}

EffectItem* MagicItem::GetEffect(const uint32_t aEffectId) noexcept
{
    for (EffectItem* pEffect : listOfEffects)
    {
        if (pEffect->pEffectSetting->formID == aEffectId)
            return pEffect;
    }

    return nullptr;
}
