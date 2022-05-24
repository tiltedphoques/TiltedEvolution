#include "MagicItem.h"

bool MagicItem::IsWardSpell() noexcept
{
    BGSKeyword* pMagicWard = Cast<BGSKeyword>(TESForm::GetById(0x1ea69));
    return keyword.Contains(pMagicWard);
}

bool MagicItem::IsInvisibilitySpell() noexcept
{
    BGSKeyword* pMagicInvisibility = Cast<BGSKeyword>(TESForm::GetById(0x1ea6f));
    bool result = keyword.Contains(pMagicInvisibility);
    spdlog::warn("IsInvisib {}", result);
    return result;
}
