#pragma once

#include <Misc/BSFixedString.h>
#include <Games/Skyrim/BSCore/BSTHashMap.h>

class IMenu;

class UI
{
  public:
    static UI* Get();

    bool GetMenuOpen(const BSFixedString& acName) const;
    void CloseAllMenus();

    void UnPauseAll();
    void UnPauseAll2();

    static void UnPauseAllDX();
  public:
    using Create_t = IMenu*();

    struct UIMenuEntry
    {
        IMenu* spMenu; // Actually a scaleform ptr
        Create_t* create;
    };

    char pad_0[0x110]; // Too lazy to reverse inheritance for now.
    // The menustack stores a collection of currently open menus
    GameArray<IMenu*> menuStack;
    // A map of name to the entry, the game will only set the menu instance
    // if the menu is opened, and frees it otherwise.
    creation::BSTHashMap<BSFixedString, UIMenuEntry> MenuMap;
};
