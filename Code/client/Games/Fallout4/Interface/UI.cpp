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
    return TiltedPhoques::ThisCall(s_isMenuOpen.Get(), this, acName);
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

void UI::PrintMenuMap()
{
    for (const auto& it : menuMap)
    {
        spdlog::info("{} @ {}", it.key.AsAscii(), fmt::ptr(it.value.spMenu));
    }
}

void UI::PrintActiveMenus()
{
    auto findName = [&](IMenu* apMenu) -> const char* {
        for (const auto& it : menuMap)
        {
            if (it.value.spMenu == apMenu)
                return it.key.AsAscii();
        }
        return nullptr;
    };

    for (IMenu* pMenu : MenuStack)
    {
        spdlog::info("{}", findName(pMenu));
    }
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

/* All known menus:
StreamingInstallMenu @ 0x0
PipboyOpeningSequenceMenu @ 0x0
MessageBoxMenu @ 0x0
CursorMenu @ 0x0
MainMenu @ 0x0
TerminalHolotapeMenu @ 0x0
BarterMenu @ 0x0
TerminalMenuButtons @ 0x0
SleepWaitMenu @ 0x0
PromptMenu @ 0x0
TutorialMenu @ 0x0
PowerArmorModMenu @ 0x0
TerminalMenu @ 0x0
ExamineConfirmMenu @ 0x0
TitleSequenceMenu @ 0x0
SitWaitMenu @ 0x0
RobotModMenu @ 0x0
FavoritesMenu @ 0x0
GenericMenu @ 0x0
PipboyHolotapeMenu @ 0x0
LoadingMenu @ 0x0
PowerArmorHUDMenu @ 0x0
BookMenu @ 0x0
Console @ 0x0
ButtonBarMenu @ 0x20a51ed70
ConsoleNativeUIMenu @ 0x0
PlayBinkMenu @ 0x0
LevelUpMenu @ 0x0
FaderMenu @ 0x20a0aa4f0
PauseMenu @ 0x0
CreditsMenu @ 0x0
VignetteMenu @ 0x0
ScopeMenu @ 0x0
HUDMenu @ 0x20b0b8b40
MultiActivateMenu @ 0x0
LooksMenu @ 0x0
DialogueMenu @ 0x0
LockpickingMenu @ 0x0
ExamineMenu @ 0x0
CreationClubMenu @ 0x0
Workshop_CaravanMenu @ 0x0
SPECIALMenu @ 0x0
ContainerMenu @ 0x0
VertibirdMenu @ 0x0
CookingMenu @ 0x0
PipboyMenu @ 0x0
VATSMenu @ 0x0
SafeZoneMenu @ 0x0
WorkshopMenu @ 0x0
StreamingInstallMenu @ 0x0
PipboyOpeningSequenceMenu @ 0x0
MessageBoxMenu @ 0x0
CursorMenu @ 0x0
MainMenu @ 0x0
TerminalHolotapeMenu @ 0x0
BarterMenu @ 0x0
TerminalMenuButtons @ 0x0
SleepWaitMenu @ 0x0
PromptMenu @ 0x0
TutorialMenu @ 0x0
PowerArmorModMenu @ 0x0
TerminalMenu @ 0x0
ExamineConfirmMenu @ 0x0
TitleSequenceMenu @ 0x0
SitWaitMenu @ 0x0
RobotModMenu @ 0x0
FavoritesMenu @ 0x0
GenericMenu @ 0x0
PipboyHolotapeMenu @ 0x0
LoadingMenu @ 0x0
PowerArmorHUDMenu @ 0x0
BookMenu @ 0x0
Console @ 0x0
ButtonBarMenu @ 0x20a51ed70
ConsoleNativeUIMenu @ 0x0
PlayBinkMenu @ 0x0
LevelUpMenu @ 0x0
FaderMenu @ 0x20a0aa4f0
PauseMenu @ 0x0
CreditsMenu @ 0x0
VignetteMenu @ 0x0
ScopeMenu @ 0x0
HUDMenu @ 0x20b0b8b40
MultiActivateMenu @ 0x0
LooksMenu @ 0x0
DialogueMenu @ 0x0
LockpickingMenu @ 0x0
ExamineMenu @ 0x0
CreationClubMenu @ 0x0
Workshop_CaravanMenu @ 0x0
SPECIALMenu @ 0x0
ContainerMenu @ 0x0
VertibirdMenu @ 0x0
CookingMenu @ 0x0
PipboyMenu @ 0x0
VATSMenu @ 0x0
SafeZoneMenu @ 0x0
WorkshopMenu @ 0x0
StreamingInstallMenu @ 0x0
PipboyOpeningSequenceMenu @ 0x0
MessageBoxMenu @ 0x0
CursorMenu @ 0x0
MainMenu @ 0x0
TerminalHolotapeMenu @ 0x0
BarterMenu @ 0x0
TerminalMenuButtons @ 0x0
SleepWaitMenu @ 0x0
PromptMenu @ 0x0
TutorialMenu @ 0x0
PowerArmorModMenu @ 0x0
TerminalMenu @ 0x0
ExamineConfirmMenu @ 0x0
TitleSequenceMenu @ 0x0
SitWaitMenu @ 0x0
RobotModMenu @ 0x0
FavoritesMenu @ 0x0
GenericMenu @ 0x0
PipboyHolotapeMenu @ 0x0
LoadingMenu @ 0x0
PowerArmorHUDMenu @ 0x0
BookMenu @ 0x0
Console @ 0x0
ButtonBarMenu @ 0x20a51ed70
ConsoleNativeUIMenu @ 0x0
PlayBinkMenu @ 0x0
LevelUpMenu @ 0x0
FaderMenu @ 0x20a0aa4f0
PauseMenu @ 0x0
CreditsMenu @ 0x0
VignetteMenu @ 0x0
ScopeMenu @ 0x0
HUDMenu @ 0x20b0b8b40
MultiActivateMenu @ 0x0
LooksMenu @ 0x0
DialogueMenu @ 0x0
LockpickingMenu @ 0x0
ExamineMenu @ 0x0
CreationClubMenu @ 0x0
Workshop_CaravanMenu @ 0x0
SPECIALMenu @ 0x0
ContainerMenu @ 0x0
VertibirdMenu @ 0x0
CookingMenu @ 0x0
PipboyMenu @ 0x0
VATSMenu @ 0x0
SafeZoneMenu @ 0x0
WorkshopMenu @ 0x0
StreamingInstallMenu @ 0x0
PipboyOpeningSequenceMenu @ 0x0
MessageBoxMenu @ 0x0
CursorMenu @ 0x0
MainMenu @ 0x0
TerminalHolotapeMenu @ 0x0
BarterMenu @ 0x0
TerminalMenuButtons @ 0x0
SleepWaitMenu @ 0x0
PromptMenu @ 0x0
TutorialMenu @ 0x0
PowerArmorModMenu @ 0x0
TerminalMenu @ 0x0
ExamineConfirmMenu @ 0x0
TitleSequenceMenu @ 0x0
SitWaitMenu @ 0x0
RobotModMenu @ 0x0
FavoritesMenu @ 0x0
GenericMenu @ 0x0
PipboyHolotapeMenu @ 0x0
LoadingMenu @ 0x0
PowerArmorHUDMenu @ 0x0
BookMenu @ 0x0
Console @ 0x0
ButtonBarMenu @ 0x20a51ed70
ConsoleNativeUIMenu @ 0x0
PlayBinkMenu @ 0x0
LevelUpMenu @ 0x0
FaderMenu @ 0x20a0aa4f0
PauseMenu @ 0x0
CreditsMenu @ 0x0
VignetteMenu @ 0x0
ScopeMenu @ 0x0
HUDMenu @ 0x20b0b8b40
MultiActivateMenu @ 0x0
LooksMenu @ 0x0
DialogueMenu @ 0x0
LockpickingMenu @ 0x0
ExamineMenu @ 0x0
CreationClubMenu @ 0x0
Workshop_CaravanMenu @ 0x0
SPECIALMenu @ 0x0
ContainerMenu @ 0x0
VertibirdMenu @ 0x0
CookingMenu @ 0x0
PipboyMenu @ 0x0
VATSMenu @ 0x0
SafeZoneMenu @ 0x0
WorkshopMenu @ 0x0
*/

static constexpr const char* kAllowList[] = {"ConsoleNativeUIMenu", "Console",        "PauseMenu",
                                             /* "PipboyMenu",*/     "TerminalMenu",   "SleepWaitMenu",
                                             "LockpickingMenu",     "MessageBoxMenu", "WorkshopMenu"};

static void (*UI_AddToMenuStack_Real)(UI*, UI::UIMenuEntry*);

void UI_AddToMenuStack(UI* apSelf, UI::UIMenuEntry* menuEntry)
{
    if (!menuEntry->spMenu)
        return;

    if (!World::Get().GetTransport().IsConnected())
        return UI_AddToMenuStack_Real(apSelf, menuEntry);

    // NOTE(Force): could also compare by RTTI later on...
    for (const char* item : kAllowList)
    {
        if (auto* pMenu = apSelf->FindMenuByName(item))
        {
            if (pMenu == menuEntry->spMenu)
                UnfreezeMenu(menuEntry->spMenu);
        }
    }

    UI_AddToMenuStack_Real(apSelf, menuEntry);
}

static TiltedPhoques::Initializer s_uiHooks([]() {
    const VersionDbPtr<uint8_t> mainMenuCtor(1079381);
    TiltedPhoques::Put<uint16_t>(mainMenuCtor.Get() + 0x2DC, 0xE990);

    // Remove engagement check, jump directly into main state.
    TiltedPhoques::Nop(mainMenuCtor.Get() + 0x279, 14); // 0x1412A01A9

    // nuke entire isinputallowed stuff
    const VersionDbPtr<uint8_t> mainMenuLoop(1512372);
    TiltedPhoques::Nop(mainMenuLoop.Get() + (0x2D - 5), 0x29); // 0x1412A0E48

    struct C : TiltedPhoques::CodeGenerator
    {
        C(uint8_t* loc)
        {
            mov(r8d, 0); // hide the prompt
            jmp_S(loc + 8);
        }
    } gen(mainMenuLoop.Get() + 0x51);
    TiltedPhoques::Jump(mainMenuLoop.Get() + 0x51, gen.getCode());
    TiltedPhoques::Nop(mainMenuLoop.Get() + 0x51 + 5, 3);

    const VersionDbPtr<uint8_t> handleInput(1001404);
    TiltedPhoques::Nop(handleInput.Get() + 0x56, 10);// 0x1412A1B76

    const VersionDbPtr<uint8_t> processMenus(239711);// 0x142042F08
    TiltedPhoques::SwapCall(processMenus.Get() + 0xAD8, UI_AddToMenuStack_Real, &UI_AddToMenuStack);
});
