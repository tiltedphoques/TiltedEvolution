#pragma once

struct ActorMagicCaster;
struct MagicItem;

struct SpellCastEvent
{
    SpellCastEvent(ActorMagicCaster* apCaster, MagicItem* apSpell) 
        : pCaster(apCaster), pSpell(apSpell)
    {}

    ActorMagicCaster* pCaster;
    MagicItem* pSpell;
    // TODO: bool interrupt
};
