#if TP_FALLOUT4

#include <Games/Fallout4/Components/TESSpellList.h>
#include <Games/Memory.h>

void TESSpellList::Initialize()
{
    if (lists != nullptr)
        return;

    lists = Memory::New<Lists>();
}


#endif
