#include <TiltedOnlinePCH.h>

#include <Games/TES.h>

TES* TES::Get() noexcept
{
    POINTER_SKYRIMSE(TES*, tes, 400441);
    POINTER_FALLOUT4(TES*, tes, 1194836);

    return *tes.Get();
}

ProcessLists* ProcessLists::Get() noexcept
{
    POINTER_SKYRIMSE(ProcessLists*, processLists, 400315);
    POINTER_FALLOUT4(ProcessLists*, processLists, 1569707);

    return *processLists.Get();
}

