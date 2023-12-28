#include "MagicItem.h"

bool MagicItem::IsWardSpell() const noexcept
{
    BGSKeyword* pMagicWard = Cast<BGSKeyword>(TESForm::GetById(0x1ea69));
    return keyword.Contains(pMagicWard);
}

bool MagicItem::IsInvisibilitySpell() const noexcept
{
    BGSKeyword* pMagicInvisibility = Cast<BGSKeyword>(TESForm::GetById(0x1ea6f));
    return keyword.Contains(pMagicInvisibility);
}

bool MagicItem::IsHealingSpell() const noexcept
{
    BGSKeyword* pMagicRestoreHealth = Cast<BGSKeyword>(TESForm::GetById(0x1ceb0));
    return keyword.Contains(pMagicRestoreHealth);
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
