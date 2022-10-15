#pragma once

#include <BSCore/BSTHashMap.h>

struct UI
{
    static UI* Get();

    using TCreate = IMenu*(UIMessage*);

    struct UIMenuEntry
    {
        IMenu* spMenu; // Actually a scaleform ptr TODO: reverse that stuff.
        TCreate* create;
        void (*pfStaticUpdate)();
    };

    IMenu* FindMenuByName(const char* acName);
    bool GetMenuOpen(const BSFixedString& acName) const;

    void PrintMenuMap();
    void PrintActiveMenus();

  public:
    uint8_t pad0[0x1A8 - sizeof(GameArray<IMenu*>)];
    GameArray<IMenu*> MenuStack; // actually stores a smart ptr
    creation::BSTHashMap<BSFixedString, UIMenuEntry> menuMap;
    uint8_t pad1D8[0x250 - 0x1D8];
};

static_assert(offsetof(UI, menuMap) == 0x1A8);
static_assert(sizeof(UI) == 0x250);
