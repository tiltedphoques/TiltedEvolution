#include "MagicCaster.h"

bool MagicCaster::CastSpell(MagicItem* apSpell, TESObjectREFR* apDesiredTarget, bool abLoadCast) noexcept
{
    TP_THIS_FUNCTION(TCastSpell, bool, MagicCaster, MagicItem*, TESObjectREFR*, bool);

    POINTER_SKYRIMSE(TCastSpell, s_castSpell, 0x140567020 - 0x140000000);

    return ThisCall(s_castSpell, this, apSpell, apDesiredTarget, abLoadCast);
}

void MagicCaster::InterruptCast(bool abRefund) noexcept
{
    TP_THIS_FUNCTION(TInterruptCast, void, MagicCaster, bool);

    POINTER_SKYRIMSE(TInterruptCast, s_interruptCast, 0x1405678A0 - 0x140000000);

    ThisCall(s_interruptCast, this, abRefund);
}

