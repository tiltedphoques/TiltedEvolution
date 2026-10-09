#include <TiltedOnlinePCH.h>

#include <Games/TES.h>

TES* TES::Get() noexcept
{
    POINTER_GAME(TES*, tes, 400441, 2698044);

    return *tes.Get();
}

ProcessLists* ProcessLists::Get() noexcept
{
    POINTER_GAME(ProcessLists*, processLists, 400315, 4796160);

    return *processLists.Get();
}
