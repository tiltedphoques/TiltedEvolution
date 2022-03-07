#include "EffectItem.h"

#include <Games/Magic/MagicSystem.h>
#include <Forms/EffectSetting.h>

bool EffectItem::IsHealingEffect() noexcept
{
    return pEffectSetting->eArchetype == EffectArchetypes::ArchetypeID::VALUE_MODIFIER && data.fMagnitude > 0.0f;
}
