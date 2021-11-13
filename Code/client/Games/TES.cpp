#include <TiltedOnlinePCH.h>

#include <Games/TES.h>

TES* TES::Get() noexcept
{
    POINTER_SKYRIMSE(TES*, tes, 0x141F5B250 - 0x140000000);
    POINTER_FALLOUT4(TES*, tes, 0x145AA4288 - 0x140000000);

    return *tes.Get();
}

ProcessLists* ProcessLists::Get() noexcept
{
    POINTER_SKYRIMSE(ProcessLists*, processLists, 0x141F592D0 - 0x140000000);
    POINTER_FALLOUT4(ProcessLists*, processLists, 0x1458CEE98 - 0x140000000);

    return *processLists.Get();
}

