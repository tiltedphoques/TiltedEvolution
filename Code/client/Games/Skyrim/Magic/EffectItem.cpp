#include "EffectItem.h"

#include <Games/Magic/MagicSystem.h>
#include <Forms/EffectSetting.h>

bool EffectItem::IsHealingEffect() const noexcept
{
    return pEffectSetting->eArchetype == EffectArchetypes::ArchetypeID::kValueModifier && data.fMagnitude > 0.0f;
}

bool EffectItem::IsSummonEffect() const noexcept
{
    return pEffectSetting->eArchetype == EffectArchetypes::ArchetypeID::kSummonCreature;
}

bool EffectItem::IsSlowEffect() const noexcept
{
    return pEffectSetting->eArchetype == EffectArchetypes::ArchetypeID::kSlowTime;
}

bool EffectItem::IsInivisibilityEffect() const noexcept
{
    return pEffectSetting->eArchetype == EffectArchetypes::ArchetypeID::kInvisibility;
}

bool EffectItem::IsWerewolfEffect() const noexcept
{
    return pEffectSetting->eArchetype == EffectArchetypes::ArchetypeID::kWerewolf;
}

bool EffectItem::IsVampireLordEffect() const noexcept
{
    return pEffectSetting->eArchetype == EffectArchetypes::ArchetypeID::kVampireLord;
}

bool EffectItem::IsNightVisionEffect() const noexcept
{
    BGSKeyword* pMagicNightEye = Cast<BGSKeyword>(TESForm::GetById(0xad7c6));
    return pEffectSetting->keywordForm.Contains(pMagicNightEye);
}

