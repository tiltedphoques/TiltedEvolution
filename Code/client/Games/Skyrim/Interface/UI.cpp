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

void UI::UnPauseAllDX()
{

}

void UI::UnPauseAll()
{
    for (auto& e : MenuMap)
    {
        // The game does lazy load its menus, so we cannot assume they exist.
        if (auto* pMenu = e.value.spMenu)
        {
            pMenu->ClearFlag(IMenu::UI_MENU_FLAGS::kPausesGame);
        }

        spdlog::info("Menu name: {}", e.key.AsAscii());
    }
}

void UI::UnPauseAll2()
{
    for (auto& e : menuStack)
    {
        spdlog::info("Menu {}", e->uiMenuFlags);
    }
}
