
#include <Games/Skyrim/Interface/IMenu.h>

void IMenu::SetFlag(uint32_t auiFlag)
{
    uiMenuFlags |= auiFlag;
}

void IMenu::ClearFlag(uint32_t auiFlag)
{
    uiMenuFlags &= ~auiFlag;
}
