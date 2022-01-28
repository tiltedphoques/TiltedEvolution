
#include "Interface/MenuControls.h"

void MenuControls::SetToggle(bool b)
{
    canBeOpened = b;
}

MenuControls* MenuControls::GetInstance()
{
    POINTER_SKYRIMSE(MenuControls*, s_instance, 0x142F9AB08 - 0x140000000);
    return *(s_instance.Get());
}
