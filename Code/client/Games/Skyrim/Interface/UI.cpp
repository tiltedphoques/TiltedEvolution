#include <Games/Skyrim/BSGraphics/BSGraphicsRenderer.h>
#include <Games/Skyrim/Interface/IMenu.h>
#include <Games/Skyrim/Interface/UI.h>
#include <Misc/BSFixedString.h>
#include <TiltedOnlinePCH.h>
#include <World.h>

#include "Sdk/Scaleform/GFxMovieDef.h"
#include "Sdk/Scaleform/GFxPlayer.h"
#include "Sdk/Scaleform/Render/Render_Viewport.h"

namespace
{
// TODO(Force): Replace with niRTTI once we found all.
static constexpr const char* kMenuUnpauseAllowList[] = {
    "Console",       "Journal Menu",   "TweenMenu",     "MagicMenu",    "StatsMenu",
    "InventoryMenu", "MessageBoxMenu", "Tutorial Menu", "ContainerMenu"
    //"MapMenu", // MapMenu is disabled till we find a proper fix for first person.
};

void ClearMenuFreezeFlags(IMenu* apEntry)
{
    if (apEntry->PausesGame())
        apEntry->ClearFlag(IMenu::kPausesGame);

    if (apEntry->FreezesBackground())
        apEntry->ClearFlag(IMenu::kFreezeFrameBackground);

    if (apEntry->FreezesFramePause())
        apEntry->ClearFlag(IMenu::kFreezeFramePause);
}
} // namespace

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

int (*Scaleform_GFx_MovieImpl_SetViewport)(void*, const Scaleform::Render::Viewport*);
void (*sub_140F67210)(void*, const Scaleform::Render::RectF*);
void (*Scaleform_GFx_MovieImpl_SetViewScaleMode)(void*, Scaleform::GFx::Movie::ScaleModeType);

float (*CalculateSafeZoneInterface_X)();
float (*CalculateSafeZoneInterface_Y)();

void UI::ResizeMovies()
{
    // the games function comes with ultrawide support.
    float safeZoneX = CalculateSafeZoneInterface_X();
    float safeZoneY = CalculateSafeZoneInterface_Y();

    auto* pState = BSGraphics::State::Get();

    float v22 = (float)pState->uiBackBufferWidth - safeZoneX;
    float v23 = (float)pState->uiBackBufferHeight - safeZoneY;
    float v24 = 1.0 / (float)pState->uiBackBufferWidth;
    float v61 = v24 * v22;
    float v25 = 1.0 / (float)pState->uiBackBufferHeight;

    // https://help.autodesk.com/view/SCLFRM/ENU/?guid=__cpp_ref_06771_html
    Scaleform::Render::Viewport newVp{};
    newVp.Width = pState->uiBackBufferWidth;
    newVp.Height = pState->uiBackBufferHeight;
    //newVp.BufferHeight = pState->uiBackBufferHeight;
    //newVp.BufferWidth = pState->uiBackBufferWidth;
    //newVp.ScissorHeight = pState->uiBackBufferHeight;
    //newVp.ScissorWidth = pState->uiBackBufferWidth;
    newVp.Flags = 1065353216;
    // void* movieView = *(void**)0x142FE9150;

    // getframesize hook

    const Scaleform::Render::RectF bounds{v24, v25, safeZoneX, safeZoneY};
    for (IMenu* pMenu : menuStack)
    {
        if (pMenu->GetMovie())
        {
            Scaleform_GFx_MovieImpl_SetViewScaleMode(pMenu->GetMovie(), Scaleform::GFx::Movie::ScaleModeType::SM_ShowAll);
            sub_140F67210(pMenu->GetMovie(), &bounds);
            Scaleform_GFx_MovieImpl_SetViewport(pMenu->GetMovie(), &newVp);
        }
    }
}

void UI::DebugLogAllMenus()
{
    for (auto& e : menuStack)
    {
        spdlog::info("Menu {}", e->uiMenuFlags);
    }
}

static void* (*UI_AddToActiveQueue)(UI*, IMenu*, void*);

static void* UI_AddToActiveQueue_Hook(UI* apSelf, IMenu* apMenu, void* apFoundItem /*In reality a reference*/)
{
    if (apMenu
#if 1
        && World::Get().GetTransport().IsConnected()
#endif
        /*TODO(Force): Maybe consider some souls like option for singleplayer*/)
    {
#if 0
        if (auto* pName = apSelf->LookupMenuNameByInstance(apMenu))
        {
            spdlog::info("Menu requested {}", pName->AsAscii());
        }
#endif

        // NOTE(Force): could also compare by RTTI later on...
        for (const char* item : kMenuUnpauseAllowList)
        {
            if (auto* pMenu = apSelf->FindMenuByName(item))
            {
                if (pMenu == apMenu)
                    ClearMenuFreezeFlags(apMenu);
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



void GetRectSize(const Scaleform::Render::RectF& in, Scaleform::Render::SizeF& out)
{
    out.Width = in.x2 - in.x1;
    out.Height = in.y2 - in.y1;
}



static TiltedPhoques::Initializer s_s([]() {
    

    // pray that this doesnt fail!
    VersionDbPtr<uint8_t> ProcessHook(82082);
    TiltedPhoques::SwapCall(ProcessHook.Get() + 0x682, UI_AddToActiveQueue, &UI_AddToActiveQueue_Hook);

    // Ignore startup movie
    // TODO: Move me later.
    VersionDbPtr<uint8_t> MainInit(36548);
    TiltedPhoques::Put<uint8_t>(MainInit.Get() + 0xFE, 0xEB);

    // Some experiments:
    // POINTER_SKYRIMSE(TCallback, s_start, 13631);
    // UIMessageQueue__AddMessage_Real = s_start.Get();
    // TP_HOOK(&UIMessageQueue__AddMessage_Real, UIMessageQueue__AddMessage);

    // This kills the loading spinner
    // TiltedPhoques::Put<uint8_t>(0x1405D51C1, 0xEB);
    // TiltedPhoques::Nop(0x1405D51A2, 5);

    // use 8 threads by default!
    // TiltedPhoques::Put<uint8_t>(0x141E45770, 8);

    VersionDbPtr<uint8_t> setVP(83989);
    Scaleform_GFx_MovieImpl_SetViewport =
        static_cast<decltype(Scaleform_GFx_MovieImpl_SetViewport)>(setVP.GetPtr());

    VersionDbPtr<uint8_t> unk1(83852);
    sub_140F67210 = static_cast<decltype(sub_140F67210)>(unk1.GetPtr());

    VersionDbPtr<uint8_t> safeZoneX(82329);
    CalculateSafeZoneInterface_X = static_cast<decltype(CalculateSafeZoneInterface_X)>(safeZoneX.GetPtr());

    VersionDbPtr<uint8_t> safeZoneY(82330);
    CalculateSafeZoneInterface_Y = static_cast<decltype(CalculateSafeZoneInterface_Y)>(safeZoneY.GetPtr());

    VersionDbPtr<uint8_t> setViewScaleMode(83988);
    Scaleform_GFx_MovieImpl_SetViewScaleMode =
        static_cast<decltype(Scaleform_GFx_MovieImpl_SetViewScaleMode)>(setViewScaleMode.GetPtr());
});
