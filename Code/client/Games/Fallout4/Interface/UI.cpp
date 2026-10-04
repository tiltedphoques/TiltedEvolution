#include <TiltedOnlinePCH.h>

#include <Interface/UI.h>

UI* UI::Get()
{
    static VersionDbPtr<UI*> singleton(4796314);
    return *singleton.Get();
}

bool UI::GetMenuOpen(const BSFixedString& acName) const
{
    if (!acName.data)
        return false;
    TP_THIS_FUNCTION(TGetMenuOpen, bool, const UI, const BSFixedString&);
    static VersionDbPtr<TGetMenuOpen> getMenuOpen(2284747);
    return TiltedPhoques::ThisCall(getMenuOpen, this, acName);
}

void UI::CloseAllMenus()
{
    TP_THIS_FUNCTION(TCloseAllMenus, void, UI);
    static VersionDbPtr<TCloseAllMenus> closeAllMenus(2284769);
    TiltedPhoques::ThisCall(closeAllMenus, this);
}

void UI::DebugLogAllMenus()
{
    for (const auto* pMenu : menuStack)
    {
        if (pMenu)
            spdlog::info("Menu {} (flags {:X})", pMenu->menuName.AsAscii(), pMenu->uiMenuFlags);
    }
}
