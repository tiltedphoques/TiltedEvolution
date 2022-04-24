#pragma once

struct ActorMagicCaster;
struct MagicItem;

/**
* @brief Dispatched when a spell has been cast locally.
*/
struct SpellCastEvent
{
    SpellCastEvent(ActorMagicCaster* apCaster, MagicItem* apSpell, uint32_t aDesiredTargetID) 
        : pCaster(apCaster), pSpell(apSpell), DesiredTargetID(aDesiredTargetID)
    {}

    // TODO(cosideci): this shouldn't be a ptr, but a form id
    ActorMagicCaster* pCaster;
    MagicItem* pSpell;
    uint32_t DesiredTargetID;
};
