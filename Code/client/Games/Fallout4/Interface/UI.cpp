#include "UI.h"
#include "IMenu.h"

UI* UI::Get()
{
    POINTER_FALLOUT4(UI*, s_instance, 548588);
    return *s_instance.Get();
}

bool UI::GetMenuOpen(const BSFixedString& acName) const
{
    if (acName.data == nullptr)
        return false;

    TP_THIS_FUNCTION(TMenuSystem_IsOpen, bool, const UI, const BSFixedString&);
    POINTER_FALLOUT4(TMenuSystem_IsOpen, s_isMenuOpen, 1065115);
    return ThisCall(s_isMenuOpen.Get(), this, acName);
}

IMenu* UI::FindMenuByName(const char* acName)
{
    for (const auto& it : menuMap)
    {
        if (!strcmp(it.key.AsAscii(), acName))
            return it.value.spMenu;
    }
    return nullptr;
}

static void UnfreezeMenu(IMenu* apEntry)
{
    if (apEntry->PausesGame())
        apEntry->ClearFlag(IMenu::UIMF_PAUSES_GAME);

    if (apEntry->FreezesBackground())
        apEntry->ClearFlag(IMenu::UIMF_FREEZE_FRAME_BACKGROUND);

    if (apEntry->FreezesFramePause())
        apEntry->ClearFlag(IMenu::UIMF_FREEZE_FRAME_PAUSE);
}

static constexpr const char* kAllowList[] = {
    "Console"
};

static void* (*UI_AddToActiveQueue)(UI*, IMenu*, void*);

static void* UI_AddToActiveQueue_Hook(UI* apSelf, IMenu* apMenu, void* apFoundItem /*In reality a reference*/)
{
    if (apMenu && World::Get()
                      .GetTransport()
                      .IsConnected() /*TODO(Force): Maybe consider some souls like option for singleplayer*/)
    {

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

static TiltedPhoques::Initializer s_uiHooks([]() {
    // TODO: find AddToActiveQueue hook
});
