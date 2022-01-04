#pragma once

struct ActorMagicCaster;
struct MagicItem;

struct SpellCastEvent
{
    SpellCastEvent(ActorMagicCaster* apCaster, MagicItem* apSpell, uint32_t aDesiredTargetID) 
        : pCaster(apCaster), pSpell(apSpell), DesiredTargetID(aDesiredTargetID)
    {}

    ActorMagicCaster* pCaster;
    MagicItem* pSpell;
    uint32_t DesiredTargetID;
};
