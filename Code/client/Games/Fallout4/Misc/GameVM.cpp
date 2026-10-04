#include <TiltedOnlinePCH.h>

#include <Misc/GameVM.h>

GameVM* GameVM::Get()
{
    static VersionDbPtr<GameVM*> s_instance(4796420);
    return *s_instance.Get();
}
