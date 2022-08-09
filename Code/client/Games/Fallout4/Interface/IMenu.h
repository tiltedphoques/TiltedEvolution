#pragma once

class UserEvents
{
  public:
    enum INPUT_CONTEXT_ID : int
    {
        ICI_MAIN_GAMEPLAY = 0x0,
        ICI_BASIC_MENU_NAV = 0x1,
        ICI_THUMB_NAV = 0x2,
        ICI_VIRT_CONTROLLER = 0x3,
        ICI_CURSOR = 0x4,
        ICI_LTHUMB_CURSOR = 0x5,
        ICI_CONSOLE = 0x6,
        ICI_DEBUG_TEXT = 0x7,
        ICI_BOOK = 0x8,
        ICI_DEBUG_OVERLAY = 0x9,
        ICI_TFC = 0xA,
        ICI_DEBUG_MAP = 0xB,
        ICI_LOCKPICK = 0xC,
        ICI_VATS = 0xD,
        ICI_VATS_PLAYBACK = 0xE,
        ICI_MULTI_ACTIVATE = 0xF,
        ICI_WORKSHOP = 0x10,
        ICI_SCOPE = 0x11,
        ICI_SITWAIT = 0x12,
        ICI_LOOKSMENU = 0x13,
        ICI_WORKSHOP_ADDENDUM = 0x14,
        ICI_PAUSEMENU = 0x15,
        ICI_LEVELUPMENU = 0x16,
        ICI_LEVELUPMENU_PREVNEXT = 0x17,
        ICI_MAINMENU = 0x18,
        ICI_QUICKCONTAINERMENU = 0x19,
        ICI_SPECIALACTIVATEROLLOVER = 0x1A,
        ICI_TWOBUTTONROLLOVER = 0x1B,
        ICI_QUICKCONTAINERMENU_PERK = 0x1C,
        ICI_VERTIBIRD = 0x1D,
        ICI_PLAYBINKMENU = 0x1E,
        ICI_ROBOT_MOD_ADDENDUM = 0x1F,
        ICI_CREATION_CLUB = 0x20,
        ICI_COUNT = 0x21,
        ICI_NONE = 0x22,
    };
};

struct __declspec(align(8)) BSInputEventUser
{
    void* __vftable /*VFT*/;
    bool InputEventHandlingEnabled;
};

struct __declspec(align(8)) Scaleform_RefCountImplCore
{
    void* __vftable /*VFT*/;
    volatile int RefCount;
};


struct IMenu : Scaleform_RefCountImplCore, BSInputEventUser
{
    enum UI_MENU_FLAGS : int32_t
    {
        UIMF_PAUSES_GAME = 0x1,
        UIMF_ALWAYS_OPEN = 0x2,
        UIMF_USES_CURSOR = 0x4,
        UIMF_USES_MENU_CONTEXT = 0x8,
        UIMF_MODAL = 0x10,
        UIMF_FREEZE_FRAME_BACKGROUND = 0x20,
        UIMF_ON_STACK = 0x40,
        UIMF_DISABLE_PAUSE_MENU = 0x80,
        UIMF_REQUIRES_UPDATE = 0x100,
        UIMF_TOPMOST_RENDERED_MENU = 0x200,
        UIMF_UPDATE_USES_CURSOR = 0x400,
        UIMF_ALLOW_SAVING = 0x800,
        UIMF_RENDERS_OFFSCREEN_TARGETS = 0x1000,
        UIMF_INVENTORY_ITEM_MENU = 0x2000,
        UIMF_DONT_HIDE_CURSOR_WHEN_TOPMOST = 0x4000,
        UIMF_CUSTOM_RENDERING = 0x8000,
        UIMF_ASSIGN_CURSOR_TO_RENDERER = 0x10000,
        UIMF_APPLICATION_MENU = 0x20000,
        UIMF_HAS_BUTTON_BAR = 0x40000,
        UIMF_IS_TOP_BUTTON_BAR = 0x80000,
        UIMF_ADVANCES_UNDER_PAUSE_MENU = 0x100000,
        UIMF_RENDERS_UNDER_PAUSE_MENU = 0x200000,
        UIMF_USES_BLURRED_BACKGROUND = 0x400000,
        UIMF_COMPANION_APP_ALLOWED = 0x800000,
        UIMF_FREEZE_FRAME_PAUSE = 0x1000000,
        UIMF_SKIP_RENDER_DURING_FREEZE_FRAME_SCREENSHOT = 0x2000000,
        UIMF_LARGE_SCALEFORM_RENDER_CACHE_MODE = 0x4000000,
        UIMF_USES_MOVEMENT_TO_DIRECTION = 0x8000000,
    };

    void SetFlag(UI_MENU_FLAGS auiFlag)
    {
        int* val = reinterpret_cast<int*>(&uiMenuFlags);
        *val |= auiFlag;
    }

    void ClearFlag(UI_MENU_FLAGS auiFlag)
    {
        int* val = reinterpret_cast<int*>(&uiMenuFlags);
        *val &= ~auiFlag;
    }

    bool PausesGame() const
    {
        return uiMenuFlags & UIMF_PAUSES_GAME;
    }

    bool FreezesBackground() const
    {
        return uiMenuFlags & UIMF_FREEZE_FRAME_BACKGROUND;
    }

    bool FreezesFramePause() const
    {
        return uiMenuFlags & UIMF_FREEZE_FRAME_PAUSE;
    }

    // force the game to generate a vt
    virtual void m1() = 0;

    char menuObj[0x20 -8]; // scaleform var.
    void* pMovie;
    BSFixedString CustomRendererName;
    BSFixedString MenuName;
    UI_MENU_FLAGS uiMenuFlags;
    uint32_t AdvanceWithoutRenderCount; // atomic
    bool bPassesTopMenuTest;
    bool bMenuCanBeVisible;
    bool bHasQuadsForCumstomRenderer;
    bool bHasDoneFirstAdvanceMovie;
    uint8_t ucDepthPriority;
    UserEvents::INPUT_CONTEXT_ID eInputContext;
};

static_assert(offsetof(IMenu, IMenu::uiMenuFlags) == 0x58);
static_assert(sizeof(IMenu) == 0x70);
