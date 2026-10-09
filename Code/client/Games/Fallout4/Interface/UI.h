#pragma once

#include <Interface/IMenu.h>

struct UI
{
    static UI* Get();
    bool GetMenuOpen(const BSFixedString& acName) const;
    void CloseAllMenus();
    void DebugLogAllMenus();

    uint8_t pad0[0x190];
    GameArray<IMenu*> menuStack;
};

static_assert(offsetof(UI, menuStack) == 0x190);
