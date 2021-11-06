#pragma once

struct ActorMagicCaster;

struct InterruptCastEvent
{
    InterruptCastEvent(uint32_t aCasterFormID) : CasterFormID(aCasterFormID)
    {}

    uint32_t CasterFormID;
};

