#pragma once

struct IMenu
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

    void SetFlag(uint32_t auiFlag)
    {
        uiMenuFlags |= auiFlag;
    }
    void ClearFlag(uint32_t auiFlag)
    {
        uiMenuFlags &= ~auiFlag;
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

    uint8_t pad0[0x58];
    uint32_t uiMenuFlags;
    uint8_t pad5C[0x70 - 0x5C];
};

static_assert(sizeof(IMenu) == 0x70);
