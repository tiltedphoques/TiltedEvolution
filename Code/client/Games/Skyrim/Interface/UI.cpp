#include <TiltedOnlinePCH.h>

#include <Games/Skyrim/Interface/IMenu.h>
#include <Games/Skyrim/Interface/UI.h>

#include <Misc/BSFixedString.h>

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
    for (auto& it : MenuMap)
    {
        if (it.value.spMenu == apMenu)
            return &it.key;
    }
    return nullptr;
}

IMenu* UI::FindMenuByName(const BSFixedString& acName)
{
    for (const auto& it : MenuMap)
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

static void MakeMenuNonBlocking(IMenu* apEntry)
{
    if (apEntry->PausesGame())
        apEntry->ClearFlag(IMenu::kPausesGame);

    if (apEntry->FreezesBackground())
        apEntry->ClearFlag(IMenu::kFreezeFrameBackground);

    if (apEntry->FreezesFramePause())
        apEntry->ClearFlag(IMenu::kFreezeFramePause);
}

// Mist Menu immedeatly hit on startup (before the beth logo)
// Main Menu: hit after beth logo.
static constexpr const char* kAllowList[] = {"Console",   "Journal Menu",  "TweenMenu", "MagicMenu",
                                             "StatsMenu", "InventoryMenu", "MapMenu"};

static __int64 (*UI_GetSomething_O)(UI*, IMenu*, __int64);

static __int64 UI_GetSomething(UI* apSelf, IMenu* apEntry, __int64 a3)
{
    if (apEntry)
    {
        if (auto* pName = apSelf->LookupMenuNameByInstance(apEntry))
        {
            spdlog::info("Menu requested {}", pName->AsAscii());
        }

        for (const char* item : kAllowList)
        {
            if (auto* pMenu = apSelf->FindMenuByName(item))
            {
                if (pMenu == apEntry)
                    MakeMenuNonBlocking(apEntry);
            }
        }
    }

    return UI_GetSomething_O(apSelf, apEntry, a3);
}

static TiltedPhoques::Initializer s_s([]() {
    // pray that this doesnt fail!
    VersionDbPtr<uint8_t> ProcessHook(82082);
    TiltedPhoques::SwapCall(ProcessHook.Get() + 0x682, UI_GetSomething_O, &UI_GetSomething);

    // Ignore startup movie
    VersionDbPtr<uint8_t> MainInit(36548);
    TiltedPhoques::Put<uint8_t>(MainInit.Get() + 0xFE, 0xEB);
});
