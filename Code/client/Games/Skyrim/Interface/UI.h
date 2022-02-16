#pragma once

#include <Games/Skyrim/BSCore/BSTHashMap.h>
#include <Misc/BSFixedString.h>

class IMenu;

struct UIMessage
{
    enum UI_MESSAGE_TYPE
    {
        kUpdate = 0,
        kShow = 1,
        kReshow = 2,
        kHide = 3,
        kForceHide = 4,

        kScaleformEvent = 6,  // BSUIScaleformData
        kUserEvent = 7,       // BSUIMessageData
        kInventoryUpdate = 8, // InventoryUpdateData
        kUserProfileChange = 9,
        kMUStatusChange = 10,
        kResumeCaching = 11,
        kUpdateController = 12,
        kChatterEvent = 13
    };

    BSFixedString Menu;
    UI_MESSAGE_TYPE eType;
    uint32_t pad0C;
    void* data;
    bool isPooled;
    uint8_t pad19;
    uint16_t pad1A;
    uint32_t pad1C;
};

class UI
{
  public:
    static UI* Get();

    bool GetMenuOpen(const BSFixedString& acName) const;
    void CloseAllMenus();

    void UnPauseAll();
    void UnPauseAll2();

    void UnPauseAllDX();

    IMenu* FindMenuByName(const BSFixedString& acName);
    BSFixedString* LookupMenuNameByInstance(IMenu* apMenu);

  public:
    using Create_t = IMenu*(UIMessage*);

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

    // Wrong offsets from here...
    uint64_t spinLock;
    uint32_t numPausesGame;                // 160 (= 0) += 1 if (imenu->flags & 0x00001)
    uint32_t numItemMenus;                 // 164 (= 0) += 1 if (imenu->flags & 0x02000)
    uint32_t numDisablePauseMenu;          // 168 (= 0) += 1 if (imenu->flags & 0x00080)
    uint32_t numAllowSaving;               // 16C (= 0) += 1 if (imenu->flags & 0x00800)
    uint32_t numDontHideCursorWhenTopmost; // 170 (= 0) += 1 if (imenu->flags & 0x04000)
    uint32_t numCustomRendering;           // 174 (= 0) += 1 if (imenu->flags & 0x08000)
    uint32_t numApplicationMenus;          // 178 (= 0) += 1 if (imenu->flags & 0x20000)
    bool modal;                            // 17C (= 0)  = 1 if (imenu->flags & 0x00010)
};

// constexpr auto x = offsetof(UI, UI::numPausesGame);

// static_assert(offsetof(UI, UI::numPausesGame) == 0x160);
