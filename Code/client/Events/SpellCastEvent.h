#pragma once

struct SpellCastEvent
{
    SpellCastEvent(ActorMagicCaster* apCaster)
        : pCaster(apCaster)
    {}

    ActorMagicCaster* pCaster;
};
