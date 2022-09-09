#pragma once

struct MiddleProcess;

struct AIProcess
{
    void KnockExplosion(Actor* apActor, const NiPoint3* aSourceLocation, float afMagnitude);

    uint8_t unk0[0x8];
    MiddleProcess* middleProcess;
};

static_assert(offsetof(AIProcess, middleProcess) == 0x8);
