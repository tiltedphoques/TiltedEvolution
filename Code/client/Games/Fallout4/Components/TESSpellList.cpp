#include <TiltedOnlinePCH.h>

#include <Components/TESSpellList.h>
#include <Games/Memory.h>

void TESSpellList::Initialize()
{
    if (lists != nullptr)
        return;

    lists = Memory::New<Lists>();
}
