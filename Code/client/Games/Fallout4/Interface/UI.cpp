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

void UI::PrintMenuMap()
{
    for (const auto& it : menuMap)
    {
        spdlog::info("{} @ {}", it.key.AsAscii(), fmt::ptr(it.value.spMenu));
    }
}

void UI::PrintActiveMenus()
{
    auto findName = [&](IMenu * apMenu) -> const char* {
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

static constexpr const char* kAllowList[] = {"ConsoleNativeUIMenu", "Console", "PauseMenu"};

static void UnpauseMenu(UI* apSelf, IMenu* apMenu)
{
    /*
        if (apMenu && World::Get()
                      .GetTransport()
                      .IsConnected() /*TODO(Force): Maybe consider some souls like option for singleplayer*/

    if (!apMenu) return;

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

using TCallback = void(UI*, UI::UIMenuEntry*);
static TCallback* UI_AddToMenuStack_Real;

void UI_AddToMenuStack(UI* apSelf, UI::UIMenuEntry* menuEntry)
{
    if (!menuEntry->spMenu)
        return;

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
    // TODO: find AddToActiveQueue hook

    TiltedPhoques::Put<uint16_t>(0x1412A020C, 0xE990);

    // nuke entire isinputallowed stuff
    TiltedPhoques::Nop(0x1412A0E48, 0x29);

    struct C : TiltedPhoques::CodeGenerator
    {
        C(uint8_t* loc)
        {
            mov(r8d, 0); // hide the prompt
            jmp_S(loc + 8);
        }
    } gen((uint8_t*)0x1412A0E71);
    TiltedPhoques::Jump(0x1412A0E71, gen.getCode());
    TiltedPhoques::Nop(0x1412A0E71 + 5, 3);


    TiltedPhoques::Nop(0x1412A1B76, 10);
    // This yeets the engagement check
    TiltedPhoques::Nop(0x1412A01A9, 14);

    #if 0
    struct MenuPatch : TiltedPhoques::CodeGenerator
    {
        MenuPatch(uint8_t* loc)
        {
            mov(rbx, rax);
            // save for later
            mov(r14, rcx);

            mov(rcx, r13); // ui thisptr
            mov(rdx, rbx); // the menu
            call(&UnpauseMenu);

            // restore
            mov(rcx, r14);

            test(rbx, rcx);
            jmp_S(loc + 6);
        }
    } genUnpause((uint8_t*)0x142042D21);

    TiltedPhoques::Jump(0x142042D21, genUnpause.getCode());
    TiltedPhoques::Nop(0x142042D21 + 5, 1);
    #endif

    // fallout seems to be reusing menus?

    #if 0
    struct MenuPatch2 : TiltedPhoques::CodeGenerator
    {
        MenuPatch2(uint8_t* loc)
        {
            mov(rcx, r13); // ui thisptr
            mov(rdx, ptr[rsp + 0x40]); // menu ptr
            call(&UnpauseMenu);

            mov(rdx, r12);
            mov(r8d, 2);
            mov(rcx, ptr[rsp + 0x30]);
            jmp_S(loc + 14);
        }
    } genUnpause2((uint8_t*)0x142043184);
    TiltedPhoques::Jump(0x142043184, genUnpause2.getCode());
    //TiltedPhoques::Nop(0x142043189 + 5, 4);
    #endif
    TiltedPhoques::SwapCall(0x142042F08, UI_AddToMenuStack_Real, &UI_AddToMenuStack);
});
