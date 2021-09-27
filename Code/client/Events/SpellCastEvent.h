#pragma once

struct ActorMagicCaster;

struct SpellCastEvent
{
    SpellCastEvent(ActorMagicCaster* apCaster)
        : pCaster(apCaster)
    {}

    ActorMagicCaster* pCaster;
    // TODO: bool interrupt
};
