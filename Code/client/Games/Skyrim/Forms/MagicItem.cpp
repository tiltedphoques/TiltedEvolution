#include "MagicItem.h"

bool MagicItem::IsWardSpell() noexcept
{
    BGSKeyword* pMagicWard = Cast<BGSKeyword>(TESForm::GetById(0x1ea69));
    return keyword.Contains(pMagicWard);
}

bool MagicItem::IsInvisibilitySpell() noexcept
{
    BGSKeyword* pMagicInvisibility = Cast<BGSKeyword>(TESForm::GetById(0x1ea6f));
    return keyword.Contains(pMagicInvisibility);
}

bool MagicItem::IsHealingSpell() noexcept
{
    BGSKeyword* pMagicRestoreHealth = Cast<BGSKeyword>(TESForm::GetById(0x1ceb0));
    return keyword.Contains(pMagicRestoreHealth);
}
