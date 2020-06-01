#pragma once

#if TP_FALLOUT4

struct MiddleProcess;

struct ProcessManager
{
    uint8_t unk0[0x8];
    MiddleProcess* middleProcess;
};

static_assert(offsetof(ProcessManager, middleProcess) == 0x8);


#endif
