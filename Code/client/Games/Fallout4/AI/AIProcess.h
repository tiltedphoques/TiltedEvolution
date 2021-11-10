#pragma once

struct MiddleProcess;

struct AIProcess
{
    uint8_t unk0[0x8];
    MiddleProcess* middleProcess;
};

static_assert(offsetof(AIProcess, middleProcess) == 0x8);
