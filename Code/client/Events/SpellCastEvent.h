#pragma once

struct ActorMagicCaster;

/**
* @brief Dispatched when a spell has been cast locally.
*/
struct SpellCastEvent
{
    SpellCastEvent(ActorMagicCaster* apCaster, uint32_t aSpellId, uint32_t aDesiredTargetID) 
        : pCaster(apCaster), SpellId(aSpellId), DesiredTargetID(aDesiredTargetID)
    {}

    ActorMagicCaster* pCaster;
    uint32_t SpellId;
    uint32_t DesiredTargetID;
};
