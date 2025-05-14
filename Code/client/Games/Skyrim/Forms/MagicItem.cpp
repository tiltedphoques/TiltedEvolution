#include "MagicItem.h"

bool MagicItem::IsWardSpell() const noexcept
{
    BGSKeyword* pMagicWard = Cast<BGSKeyword>(TESForm::GetById(0x1ea69));
    if (keyword.count == 0)
        spdlog::debug(__FUNCTION__ ": correcting BGSKeywordForm::Contains() bug for zero-keyword {:x}, {}", this->formID, this->fullName.value.AsAscii());
    return keyword.count > 0 && keyword.Contains(pMagicWard);
}

bool MagicItem::IsInvisibilitySpell() const noexcept
{
    BGSKeyword* pMagicInvisibility = Cast<BGSKeyword>(TESForm::GetById(0x1ea6f));
    if (keyword.count == 0)
        spdlog::debug(__FUNCTION__ ": correcting BGSKeywordForm::Contains() bug for zero-keyword {:x}, {}", this->formID, this->fullName.value.AsAscii());
    return keyword.count > 0 && keyword.Contains(pMagicInvisibility);
}

bool MagicItem::IsHealingSpell() const noexcept
{
    BGSKeyword* pMagicRestoreHealth = Cast<BGSKeyword>(TESForm::GetById(0x1ceb0));
    if (keyword.count == 0)
        spdlog::debug(__FUNCTION__ ": correcting BGSKeywordForm::Contains() bug for zero-keyword {:x}, {}", this->formID, this->fullName.value.AsAscii());
    return keyword.count > 0 && keyword.Contains(pMagicRestoreHealth);
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
