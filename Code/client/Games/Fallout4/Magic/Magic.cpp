#include <Forms/MagicItem.h>
#include <Magic/MagicCaster.h>

// Fallout 4 has no ward, bound weapon or restore health keywords comparable to
// Skyrim's, so these classify spells by effect archetype only.

namespace
{
bool HasArchetype(const GameArray<EffectItem*>& acEffects, EffectArchetypes::ArchetypeID aArchetype) noexcept
{
    for (const EffectItem* pEffect : acEffects)
    {
        if (pEffect && pEffect->pEffectSetting && pEffect->pEffectSetting->eArchetype == aArchetype)
            return true;
    }
    return false;
}
} // namespace

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
    return false;
}

bool EffectItem::IsVampireLordEffect() const noexcept
{
    return false;
}

bool EffectItem::IsNightVisionEffect() const noexcept
{
    return pEffectSetting->eArchetype == EffectArchetypes::ArchetypeID::kNightEye;
}

bool MagicItem::IsWardSpell() const noexcept
{
    return false;
}

bool MagicItem::IsInvisibilitySpell() const noexcept
{
    return HasArchetype(listOfEffects, EffectArchetypes::ArchetypeID::kInvisibility);
}

bool MagicItem::IsHealingSpell() const noexcept
{
    for (const EffectItem* pEffect : listOfEffects)
    {
        if (pEffect && pEffect->pEffectSetting && pEffect->IsHealingEffect())
            return true;
    }
    return false;
}

bool MagicItem::IsBuffSpell() const noexcept
{
    return false;
}

bool MagicItem::IsBoundWeaponSpell() noexcept
{
    return HasArchetype(listOfEffects, EffectArchetypes::ArchetypeID::kBoundWeapon);
}

bool MagicItem::HasSummonEffect() const noexcept
{
    return HasArchetype(listOfEffects, EffectArchetypes::ArchetypeID::kSummonCreature);
}

EffectItem* MagicItem::GetEffect(const uint32_t aEffectId) noexcept
{
    for (EffectItem* pEffect : listOfEffects)
    {
        if (pEffect && pEffect->pEffectSetting && pEffect->pEffectSetting->formID == aEffectId)
            return pEffect;
    }
    return nullptr;
}

void MagicCaster::InterruptCast() noexcept
{
    InterruptCastImpl(false);
}
