
// The game calls it statsmenu.
#include <Interface/Menus/SkillsMenu.h>

// Where the patched instruction sits inside its function. SkyrimVR 1.4.15 and Anniversary Edition
// were compiled from different revisions, so an offset is only valid for the build it was read
// from. Tools/vr_addresses/patches.mjs checks both columns against the two exes.
#if TP_SKYRIMVR
constexpr size_t kMainThreadCheck = 0x91C;
constexpr size_t kFreezeFrameFlag = 0xBB6;
constexpr size_t kMenuUpdateCheck = 0x109C;
constexpr size_t kCanProcessGuard = 0x98;
#else
constexpr size_t kMainThreadCheck = 0x84E;
constexpr size_t kFreezeFrameFlag = 0xA10;
constexpr size_t kMenuUpdateCheck = 0x1040;
constexpr size_t kCanProcessGuard = 0x46;
#endif

#if TP_SKYRIMVR
#include <Games/Skyrim/Interface/UI.h>

// VR draws the skill tree as a 3D model in the world scene (StatsMenu+0xC0, a BSFadeNode that fades
// out between 500 and 1000 units). Tree view moves it about 3000 units away. Paused, the fade update
// never runs and the model stays visible; unpaused it is faded to 0 and never drawn, so the tree
// renders black. Keep it fully faded in while the menu does not pause the game.
constexpr size_t kStatsModel = 0xC0;
constexpr size_t kFadeNodeCurrentFade = 0x158;
constexpr size_t kAvObjectFlags = 0x10C;
constexpr uint32_t kIgnoreFade = 0x8000;

TP_THIS_FUNCTION(TStatsProcessMessage, uint32_t, IMenu, UIMessage&);
static TStatsProcessMessage* RealStatsProcessMessage;

// The main thread check nopped above (kMainThreadCheck) lets the menu's update run from the UI job, which
// with the game unpaused is a worker thread. That update also moves the tree model (ProcessMessage+0x124B,
// NiAVObject::Update on StatsMenu+0xC0) while the main thread culls and draws it, so the stars and skill
// and perk names blink depending on where the player looks. Off the main thread, keep the update and run it
// from the start of the next main loop instead (Main::Update, see SkyrimVM64.cpp), before anything draws.
// The game writes shader controllers from here with buffer select 0 (the slot being drawn); at frame start
// the default select 1 writes the slot the main loop's parity flip is about to make current.
constexpr uintptr_t kTreeModelUpdateCall = 0x8ED62B;
constexpr uintptr_t kMainThreadOwner = 0x137B630;

struct NiUpdateData
{
    float time;
    uint32_t flags;
};

static void (*RealTreeModelUpdate)(void*, NiUpdateData*);
static std::mutex s_treeUpdateLock;
static void* s_pendingTreeModel = nullptr;
static NiUpdateData s_pendingTreeData{};

static bool IsMainThread()
{
    const auto* pOwner = reinterpret_cast<const uint8_t* (*)()>(reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr)) + kMainThreadOwner)();
    return GetCurrentThreadId() == *reinterpret_cast<const DWORD*>(pOwner + 0x108);
}

static void HookTreeModelUpdate(void* apModel, NiUpdateData* apData)
{
    if (IsMainThread())
    {
        RealTreeModelUpdate(apModel, apData);
        return;
    }

    std::scoped_lock lock(s_treeUpdateLock);
    s_pendingTreeModel = apModel;
    s_pendingTreeData = *apData;
}

// Called from the start of Main::Update.
void RunDeferredSkillsTreeUpdate()
{
    std::scoped_lock lock(s_treeUpdateLock);
    if (!s_pendingTreeModel)
        return;

    RealTreeModelUpdate(s_pendingTreeModel, &s_pendingTreeData);
    s_pendingTreeModel = nullptr;
}

static uint32_t HookStatsProcessMessage(IMenu* apThis, UIMessage& aMessage)
{
    const uint32_t result = TiltedPhoques::ThisCall(RealStatsProcessMessage, apThis, aMessage);

    if (aMessage.eType == UIMessage::kHide || aMessage.eType == UIMessage::kForceHide)
    {
        std::scoped_lock lock(s_treeUpdateLock);
        s_pendingTreeModel = nullptr;
    }

    if (!apThis->PausesGame())
    {
        if (auto* pModel = *reinterpret_cast<uint8_t**>(reinterpret_cast<uint8_t*>(apThis) + kStatsModel))
        {
            *reinterpret_cast<float*>(pModel + kFadeNodeCurrentFade) = 1.f;
            *reinterpret_cast<uint32_t*>(pModel + kAvObjectFlags) |= kIgnoreFade;
        }
    }

    return result;
}
#endif

static TiltedPhoques::Initializer s_skillsMenuInit(
    []()
    {
        // https://github.com/Vermunds/SkyrimSoulsRE/blob/master/src/Menus/StatsMenuEx.cpp
        // Hoooks from souls RE
        // Fix for menu not appearing
        VersionDbPtr<uint8_t> ProcessMessage(52510);
        TiltedPhoques::Nop(ProcessMessage.Get() + kMainThreadCheck, 6);
        // Prevent setting kFreezeFrameBackground flag
        TiltedPhoques::Nop(ProcessMessage.Get() + kFreezeFrameFlag, 4);
        // Keep the menu updated
        TiltedPhoques::Nop(ProcessMessage.Get() + kMenuUpdateCheck, 2);

        // Fix for controls not working. Both Nops together cover one 6 byte jnz.
        VersionDbPtr<uint8_t> controlPatch(52518);
        TiltedPhoques::Nop(controlPatch.Get() + kCanProcessGuard, 4);
        TiltedPhoques::Nop(controlPatch.Get() + kCanProcessGuard + 4, 2);

#if TP_SKYRIMVR
        RealStatsProcessMessage = reinterpret_cast<TStatsProcessMessage*>(ProcessMessage.Get());
        TP_HOOK(&RealStatsProcessMessage, HookStatsProcessMessage);
        TiltedPhoques::SwapCall(reinterpret_cast<uint8_t*>(GetModuleHandleW(nullptr)) + kTreeModelUpdateCall, RealTreeModelUpdate, &HookTreeModelUpdate);
#endif
    });
