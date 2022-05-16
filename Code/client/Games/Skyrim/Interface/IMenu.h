#pragma once

struct GRefCountImplCore
{
    virtual ~GRefCountImplCore() = default;

    static void CheckInvalidDelete(GRefCountImplCore*)
    {
    }

    [[nodiscard]] constexpr uint32_t GetRefCount() const noexcept
    {
        return m_refCount;
    }

  protected:
    uint32_t m_refCount{1};
    std::uint32_t _pad0C{0};
};

struct GRefCountImpl : GRefCountImplCore
{
    virtual ~GRefCountImpl() = default; // 00
};
static_assert(sizeof(GRefCountImpl) == 0x10);

template <class Base, std::uint32_t StatType>
struct GRefCountBaseStatImpl : Base
{
    // GFC_MEMORY_REDEFINE_NEW_IMPL(Base, GFC_REFCOUNTALLOC_CHECK_DELETE, StatType);
};

template <class T, std::uint32_t STAT>
struct GRefCountBase : GRefCountBaseStatImpl<GRefCountImpl, STAT>
{
    enum
    {
        kStatType = STAT
    };
};

struct GStatGroups
{
    // _Mem for GMemoryStat.
    // _Tks for GTimerStat.
    // _Cnt for GCounterStat.
    enum GStatGroup : uint32_t
    {
        kGStatGroup_Default = 0,

        kGStatGroup_Core = 16,
        kGStatGroup_Renderer = 1 << 6,
        kGStatGroup_RenderGen = 2 << 6,

        kGStatGroup_GFxFontCache = 3 << 6,
        kGStatGroup_GFxMovieData = 4 << 6,
        kGStatGroup_GFxMovieView = 5 << 6,
        kGStatGroup_GFxRenderCache = 6 << 6,
        kGStatGroup_GFxPlayer = 7 << 6,
        kGStatGroup_GFxIME = 8 << 6,

        // General memory
        kGStat_Mem = kGStatGroup_Default + 1,
        kGStat_Default_Mem,
        kGStat_Image_Mem,
        kGStat_Sound_Mem,
        kGStat_String_Mem,
        kGStat_Video_Mem,

        // Memory allocated for debugging purposes.
        kGStat_Debug_Mem,
        kGStat_DebugHUD_Mem,
        kGStat_DebugTracker_Mem,
        kGStat_StatBag_Mem,

        // Core entries
        kGStatHeap_Start = kGStatGroup_Core,
        // 16 slots for HeapSummary

        // How many entries we support by default
        kGStat_MaxId = 64 << 6, // 64 * 64 = 4096
        kGStat_EntryCount = 512
    };
};

struct FxDelegateHandler : GRefCountBase<FxDelegateHandler, GStatGroups::kGStat_Default_Mem>
{
    using CallbackFn = void(const void*);

    class CallbackProcessor
    {
      public:
        virtual ~CallbackProcessor() = default;
        virtual void Process(const void* apMethodName, CallbackFn* apMethod) = 0;
    };
    static_assert(sizeof(CallbackProcessor) == 0x8);

    ~FxDelegateHandler() override = default; // 00

    // add
    virtual void Accept(CallbackProcessor* a_cbReg) = 0; // 01
};

enum class UI_MESSAGE_RESULTS
{
    kHandled = 0,
    kIgnore = 1,
    kPassOn = 2
};

struct UIMessage;

struct IMenu : FxDelegateHandler
{
    virtual ~IMenu() = default;

    void Accept(CallbackProcessor* apProcessor) override;

    // add
    virtual void PostCreate() = 0;
    virtual void Unk_03(void) = 0;
    virtual UI_MESSAGE_RESULTS ProcessMessage(UIMessage& aMessage) = 0;
    virtual void AdvanceMovie(float aInterval, uint32_t aCurrentTime) = 0;
    virtual void PostDisplay() = 0;
    virtual void PreDisplay() = 0;
    virtual void RefreshPlatform() = 0;

    enum UI_MENU_FLAGS : uint32_t
    {
        // Multiples of two
        kNone = 0x0,
        kPausesGame = 0x1,
        kAlwaysOpen = 0x2,
        kUsesCursor = 0x4,
        kUsesMenuContext = 0x8,
        kModal = 0x10,
        kFreezeFrameBackground = 0x20,
        kOnStack = 0x40,
        kDisablePauseMenu = 0x80,
        kRequiresUpdate = 0x100,
        kTopmostRenderedMenu = 0x200,
        kUpdateUsesCursor = 0x400,
        kAllowSaving = 0x800,
        kRendersOffscreenTargets = 0x1000,
        kInventoryItemMenu = 0x2000,
        kDontHideCursorWhenTopmost = 0x4000,
        kCustomRendering = 0x8000,
        kAssignCursorToRenderer = 0x10000,
        kApplicationMenu = 0x20000,
        kHasButtonBar = 0x40000,
        kIsTopButtonBar = 0x80000,
        kAdvancesUnderPauseMenu = 0x100000,
        kRendersUnderPauseMenu = 0x200000,
        kUsesBlurredBackground = 0x400000,
        kCompanionAppAllowed = 0x800000,
        kFreezeFramePause = 0x1000000,
        kSkipRenderDuringFreezeFrameScreenshot = 0x2000000,
        kLargeScaleformRenderCacheMode = 0x4000000,
        kUsesMovementToDirection = 0x8000000,
    };

    void SetFlag(uint32_t auiFlag);
    void ClearFlag(uint32_t auiFlag);

    bool PausesGame() const
    {
        return uiMenuFlags & kPausesGame;
    }

    bool FreezesBackground() const
    {
        return uiMenuFlags & kFreezeFrameBackground;
    }

    bool FreezesFramePause() const
    {
        return uiMenuFlags & kFreezeFramePause;
    }

    void* uiMovie{nullptr};
    int8_t depthPriority{3};
    uint8_t pad19{0};
    uint16_t pad20{0};
    uint32_t uiMenuFlags;
    uint32_t eInputContext;
    std::uint32_t pad24{0};
    void* fxDelegate{nullptr};
};

constexpr auto x = offsetof(IMenu, IMenu::uiMenuFlags);

static_assert(offsetof(IMenu, IMenu::uiMenuFlags) == 0x1C);
