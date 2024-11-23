#include <TiltedOnlinePCH.h>

#include <Games/TES.h>

TES* TES::Get() noexcept
{
    POINTER_SKYRIMSE(TES*, tes, 400441);

    return *tes.Get();
}

ProcessLists* ProcessLists::Get() noexcept
{
    POINTER_SKYRIMSE(ProcessLists*, processLists, 400315);

    return *processLists.Get();
}
