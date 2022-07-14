#include <Games/Skyrim/Interface/IMenu.h>
#include <Games/Skyrim/Interface/UI.h>
#include <Misc/BSFixedString.h>
#include <TiltedOnlinePCH.h>

#include <World.h>

static bool g_RequestUnpauseAll{false};

UI* UI::Get()
{
    POINTER_SKYRIMSE(UI*, s_instance, 400327);
    POINTER_FALLOUT4(UI*, s_instance, 0x1458D0898 - 0x140000000);

    return *s_instance.Get();
}

bool UI::GetMenuOpen(const BSFixedString& acName) const
{
    if (acName.data == nullptr)
        return false;

    TP_THIS_FUNCTION(TMenuSystem_IsOpen, bool, const UI, const BSFixedString&);

    POINTER_SKYRIMSE(TMenuSystem_IsOpen, s_isMenuOpen, 82074);
    POINTER_FALLOUT4(TMenuSystem_IsOpen, s_isMenuOpen, 0x142042160 - 0x140000000);

    return ThisCall(s_isMenuOpen.Get(), this, acName);
}

void UI::CloseAllMenus()
{
    TP_THIS_FUNCTION(TUI_CloseAll, void, const UI);
    POINTER_SKYRIMSE(TUI_CloseAll, s_CloseAll, 82088);

    ThisCall(s_CloseAll.Get(), this);
}

BSFixedString* UI::LookupMenuNameByInstance(IMenu* apMenu)
{
    for (auto& it : menuMap)
    {
        if (it.value.spMenu == apMenu)
            return &it.key;
    }
    return nullptr;
}

IMenu* UI::FindMenuByName(const BSFixedString& acName)
{
    for (const auto& it : menuMap)
    {
        if (it.key == acName)
            return it.value.spMenu;
    }
    return nullptr;
}

void UI::DebugLogAllMenus()
{
    for (auto& e : menuStack)
    {
        spdlog::info("Menu {}", e->uiMenuFlags);
    }
}

static void UnfreezeMenu(IMenu* apEntry)
{
    if (apEntry->PausesGame())
        apEntry->ClearFlag(IMenu::kPausesGame);

    if (apEntry->FreezesBackground())
        apEntry->ClearFlag(IMenu::kFreezeFrameBackground);

    if (apEntry->FreezesFramePause())
        apEntry->ClearFlag(IMenu::kFreezeFramePause);
}

static constexpr const char* kAllowList[] = {
    "TweenMenu", "MagicMenu", "StatsMenu", "InventoryMenu", "MessageBoxMenu", "ContainerMenu",
    "FavoritesMenu", "Tutorial Menu", "Console"
    //"MapMenu", // MapMenu is disabled till we find a proper fix for first person.
    //"Journal Menu", // Journal menu, aka pause menu, is disabled until we find a fix for manual save crashing while unpaused.
};

static void* (*UI_AddToActiveQueue)(UI*, IMenu*, void*);

static void* UI_AddToActiveQueue_Hook(UI* apSelf, IMenu* apMenu, void* apFoundItem /*In reality a reference*/)
{
    if (apMenu && World::Get()
                      .GetTransport()
                      .IsConnected() /*TODO(Force): Maybe consider some souls like option for singleplayer*/)
    {
#if 0
        if (auto* pName = apSelf->LookupMenuNameByInstance(apEntry))
        {
            spdlog::info("Menu requested {}", pName->AsAscii());
        }
#endif

        // NOTE(Force): could also compare by RTTI later on...
        for (const char* item : kAllowList)
        {
            if (auto* pMenu = apSelf->FindMenuByName(item))
            {
                if (pMenu == apMenu)
                    UnfreezeMenu(apMenu);
            }
        }
    }

    return UI_AddToActiveQueue(apSelf, apMenu, apFoundItem);
}

using TCallback = void(void*, const BSFixedString*, uint32_t, void*);
static TCallback* UIMessageQueue__AddMessage_Real;

// Useful for debugging UI related issues.
void UIMessageQueue__AddMessage(void* a1, const BSFixedString* a2, UIMessage::UI_MESSAGE_TYPE a3, void* a4)
{
    spdlog::info("Adding Message {} with prio {} from {}", a2->AsAscii(), a3, fmt::ptr(_ReturnAddress()));
    UIMessageQueue__AddMessage_Real(a1, a2, a3, a4);
}

static TiltedPhoques::Initializer s_s([]() {
    // pray that this doesnt fail!
    VersionDbPtr<uint8_t> ProcessHook(82082);
    TiltedPhoques::SwapCall(ProcessHook.Get() + 0x682, UI_AddToActiveQueue, &UI_AddToActiveQueue_Hook);

    // Ignore startup movie
    // TODO: Move me later.
    VersionDbPtr<uint8_t> MainInit(36548);
    TiltedPhoques::Put<uint8_t>(MainInit.Get() + 0xFE, 0xEB);

    // Credits to Skyrim Souls RE for this fix.
    // Allows the favorites menu to be numbered during connect.
    VersionDbPtr<uint8_t> FavoritesCanProcess(51538);
    TiltedPhoques::Put<uint16_t>(FavoritesCanProcess.Get() + 0x15, 0x9090);

    // Some experiments:
    // POINTER_SKYRIMSE(TCallback, s_start, 13631);
    // UIMessageQueue__AddMessage_Real = s_start.Get();
    // TP_HOOK(&UIMessageQueue__AddMessage_Real, UIMessageQueue__AddMessage);

    // This kills the loading spinner
    // TiltedPhoques::Put<uint8_t>(0x1405D51C1, 0xEB);
    // TiltedPhoques::Nop(0x1405D51A2, 5);

    // use 8 threads by default!
    // TiltedPhoques::Put<uint8_t>(0x141E45770, 8);
});
