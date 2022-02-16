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

static void ClearFlags(IMenu* apEntry)
{
    if (apEntry->PausesGame())
        apEntry->ClearFlag(IMenu::kPausesGame);

    if (apEntry->FreezesBackground())
        apEntry->ClearFlag(IMenu::kFreezeFrameBackground);

    if (apEntry->FreezesFramePause())
        apEntry->ClearFlag(IMenu::kFreezeFramePause);
}

static __int64 (*OldFn)(UI*, IMenu*, __int64 a3);

// Unpause works very well for Console


static constexpr const char* kAllowList[] = {"Console", "Journal Menu",   "TweenMenu", "MagicMenu",
                                             /*"MapMenu", "StatsMenu",*/ "InventoryMenu"};

static __int64 NewFn(UI* apSelf, IMenu* apEntry, __int64 a3)
{
    if (apEntry)
    {
        // Mist Menu immedeatly hit on startup (before the beth logo)
        // Main Menu: hit after beth logo.

        if (auto* pName = apSelf->LookupMenuNameByInstance(apEntry))
        {
            spdlog::info("Menu requested {}", pName->AsAscii());
        }

        for (const char* item : kAllowList)
        {
            if (auto* pMenu = apSelf->FindMenuByName(item))
            {
                if (pMenu == apEntry)
                    ClearFlags(apEntry);
            }

        }

        // ClearFlags(apEntry);
    }

    return OldFn(apSelf, apEntry, a3);
}

static void (*sub_140F06A80_0)(void*, IMenu*);

static void sub_140F06A80(void* a1, IMenu* apEntry)
{
    if (apEntry)
        ClearFlags(apEntry);

    sub_140F06A80_0(a1, apEntry);
}

static void (*BSTreeManager__Update_O)(void*, void*, bool);

void BSTreeManager__Update(void* treeManager, void* a2, bool abMenuMode)
{
    if (!a2)
        return;

    BSTreeManager__Update_O(treeManager, a2, abMenuMode);
}

static TiltedPhoques::Initializer s_s([]() {
    TiltedPhoques::SwapCall(0x140F05232, OldFn, &NewFn);
    // TiltedPhoques::SwapCall(0x1405D9F96, BSTreeManager__Update_O, &BSTreeManager__Update);
    //
    //
    // TiltedPhoques::SwapCall(0x140F054AC, sub_140F06A80_0, &sub_140F06A80);

    struct C : TiltedPhoques::CodeGenerator
    {
        C()
        {
            mov(rdi, ptr[rdx + 0x18]);
            mov(edx, 0);

            cmp(rdi, 0);
            jz("exit");
            db(0xCC);
            jmp_S(0x1403F6460 + 1);

            L("exit");
            add(rsp, 0x30260);
            pop(r15);
            pop(r14);
            pop(r13);
            pop(r12);
            pop(rdi);
            pop(rsi);
            pop(rbx);
            pop(rbp);
            ret();
        }
    } gen;
    // TiltedPhoques::Jump(0x1403F645C, gen.getCode());
    // TiltedPhoques::Nop(0x1403F645C + 5, 1);

    // iltedPhoques::Jump(0x1403F6484, gen.getCode());
    // TiltedPhoques::Nop(0x1403F6484 + 5, 2);
});
