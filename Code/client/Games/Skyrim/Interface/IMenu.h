#pragma once

class IMenu
{
  public:
    enum UI_MENU_FLAGS : uint32_t
    {
        kNone = 0,
        kPausesGame = 1 << 0,
        kAlwaysOpen = 1 << 1,
        kUsesCursor = 1 << 2,
        kUsesMenuContext = 1 << 3,
        kModal = 1 << 4, // prevents lower movies with this flag from advancing
        kFreezeFrameBackground = 1 << 5,
        kOnStack = 1 << 6,
        kDisablePauseMenu = 1 << 7,
        kRequiresUpdate = 1 << 8,
        kTopmostRenderedMenu = 1 << 9,
        kUpdateUsesCursor = 1 << 10,
        kAllowSaving = 1 << 11,
        kRendersOffscreenTargets = 1 << 12,
        kInventoryItemMenu = 1 << 13,
        kDontHideCursorWhenTopmost = 1 << 14,
        kCustomRendering = 1 << 15,
        kAssignCursorToRenderer = 1 << 16,
        kApplicationMenu = 1 << 17,
        kHasButtonBar = 1 << 18,
        kIsTopButtonBar = 1 << 19,
        kAdvancesUnderPauseMenu = 1 << 20,
        kRendersUnderPauseMenu = 1 << 21,
        kUsesBlurredBackground = 1 << 22,
        kCompanionAppAllowed = 1 << 23,
        kFreezeFramePause = 1 << 24,
        kSkipRenderDuringFreezeFrameScreenshot = 1 << 25,
        kLargeScaleformRenderCacheMode = 1 << 26,
        kUsesMovementToDirection = 1 << 27
    };

    void SetFlag(uint32_t auiFlag);
    void ClearFlag(uint32_t auiFlag);

  public:
    char pad_0[0x1C];
    uint32_t uiMenuFlags;
    uint32_t inputContext;

    //needs create flag?
};

//constexpr auto x = offsetof(IMenu, IMenu::uiMenuFlags);
