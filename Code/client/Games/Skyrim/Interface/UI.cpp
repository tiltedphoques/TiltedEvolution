#include <Games/Skyrim/BSGraphics/BSGraphicsRenderer.h>
#include <Games/Skyrim/Interface/IMenu.h>
#include <Games/Skyrim/Interface/UI.h>
#include <Misc/BSFixedString.h>
#include <TiltedOnlinePCH.h>

#include <World.h>

namespace
{
bool g_RequestUnpauseAll{false};

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

struct Scaleform_Render_Viewport
{
    int BufferWidth;
    int BufferHeight;
    int Left;
    int Top;
    int Width;
    int Height;
    int ScissorLeft;
    int ScissorTop;
    int ScissorWidth;
    int ScissorHeight;
    unsigned int Flags;
};

struct Scaleform_GFx_Viewport : Scaleform_Render_Viewport
{
    float Scale;
    float AspectRatio;
};

template <typename T> struct Scaleform_Render_Rect
{
    T x1;
    T y1;
    T x2;
    T y2;
};

enum Scaleform_GFx_Movie_ScaleModeType : __int32
{
    SM_NoScale = 0x0,
    SM_ShowAll = 0x1,
    SM_ExactFit = 0x2,
    SM_NoBorder = 0x3,
};

int (*Scaleform_GFx_MovieImpl_SetViewport)(void*, const Scaleform_GFx_Viewport*);
void (*sub_140F67210)(void*, const Scaleform_Render_Rect<float>*);
void (*Scaleform_GFx_MovieImpl_SetViewScaleMode)(void*, Scaleform_GFx_Movie_ScaleModeType);

float (*CalculateSafeZoneInterface_X)();
float (*CalculateSafeZoneInterface_Y)();

void UI::ResizeMovies()
{
    // the games function comes with ultrawide support.
    float safeZoneX = CalculateSafeZoneInterface_X();
    float safeZoneY = CalculateSafeZoneInterface_Y();

    auto* pState = (BSGraphics::State*)0x1430C6D90;

    float v22 = (float)pState->uiBackBufferWidth - safeZoneX;
    float v23 = (float)pState->uiBackBufferHeight - safeZoneY;
    float v24 = 1.0 / (float)pState->uiBackBufferWidth;
    float v61 = v24 * v22;
    float v25 = 1.0 / (float)pState->uiBackBufferHeight;

    Scaleform_GFx_Viewport newVp{};
    newVp.Width = pState->uiBackBufferWidth;
    newVp.Height = pState->uiBackBufferHeight;
    newVp.Flags = 1065353216;
    newVp.Scale = 1.f;
    newVp.AspectRatio = 1.f;
    // void* movieView = *(void**)0x142FE9150;

    // getframesize hook

    const Scaleform_Render_Rect<float> bounds{v24, v25, safeZoneX, safeZoneY};
    for (IMenu* pMenu : menuStack)
    {
        if (pMenu->GetMovie())
        {
            Scaleform_GFx_MovieImpl_SetViewScaleMode(pMenu->GetMovie(), Scaleform_GFx_Movie_ScaleModeType::SM_ShowAll);
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

enum Scaleform_GFx_FileTypeConstants_FileFormatType : __int32
{
    File_Unopened = 0x0,
    File_Unknown = 0x1,
    File_SWF = 0x2,
    File_GFX = 0x3,
    File_JPEG = 0xA,
    File_PNG = 0xB,
    File_GIF = 0xC,
    File_TGA = 0xD,
    File_DDS = 0xE,
    File_HDR = 0xF,
    File_BMP = 0x10,
    File_DIB = 0x11,
    File_PFM = 0x12,
    File_TIFF = 0x13,
    File_WAVE = 0x14,
    File_PVR = 0x15,
    File_ETC = 0x16,
    File_SIF = 0x17,
    File_GXT = 0x18,
    File_GTX = 0x19,
    File_GNF = 0x1A,
    File_KTX = 0x1B,
    File_NextAvail = 0x1C,
    File_DDSBC = 0xFFFD,
    File_MultiFormat = 0xFFFE,
    File_Original = 0xFFFF,
};

const struct Scaleform_GFx_ExporterInfo
{
    Scaleform_GFx_FileTypeConstants_FileFormatType Format;
    const char* pPrefix;
    const char* pSWFName;
    unsigned __int16 Version;
    unsigned int ExportFlags;
};

struct __declspec(align(4)) Scaleform_String_DataDesc
{
    unsigned __int64 Size;
    volatile int RefCount;
    char Data[1];
};

struct Scaleform_String
{
    union {
        Scaleform_String_DataDesc* pData;
        unsigned __int64 HeapTypeBits;
    };
};

struct Scaleform_GFx_ExporterInfoImpl
{
    Scaleform_GFx_ExporterInfo SI;
    Scaleform_String Prefix;
    Scaleform_String SWFName;
    char padx[0x18];
};


struct Scaleform_GFx_MovieHeaderData
{
    unsigned int FileLength;
    int Version;
    Scaleform_Render_Rect<float> FrameRect;
    float FPS;
    unsigned int FrameCount;
    unsigned int SWFFlags;
    Scaleform_GFx_ExporterInfoImpl mExporterInfo;
};

template <typename T> struct Scaleform_Render_Size
{
    T Width;
    T Height;
};

struct Scaleform_GFx_MovieDataDef_LoadTaskDataBase{
    char pad0[0x38];
};



struct Scaleform_StringLH : Scaleform_String
{
};


struct Scaleform_GFx_MovieDataDef_LoadTaskData : Scaleform_GFx_MovieDataDef_LoadTaskDataBase
{
    void* pHeap;
    void* pImageHeap;
    Scaleform_StringLH FileURL;
    Scaleform_GFx_MovieHeaderData Header;
};

void (*realp)(Scaleform_GFx_MovieDataDef_LoadTaskData*, Scaleform_GFx_MovieHeaderData*);

void GetRectSize(const Scaleform_Render_Rect<float>& in, Scaleform_Render_Size<float>& out)
{
    out.Width = in.x2 - in.x1;
    out.Height = in.y2 - in.y1;
}

// it must be something else on the left side.
void operatorEquals(Scaleform_GFx_MovieDataDef_LoadTaskData* pTaskData, Scaleform_GFx_MovieHeaderData* rhs)
{
    auto* pState = (BSGraphics::State*)0x1430C6D90;
    if (rhs->FrameRect.x2 == 25600.0000)
    {
        //rhs->FrameRect.x2 = static_cast<float>(pState->uiBackBufferWidth) * 10.f;
        //rhs->FrameRect.y2 = static_cast<float>(pState->uiBackBufferHeight) * 10.f;
    }

    // uncap menu FPS.
    rhs->FPS = 60.f;

    //rhs->FrameRect.x2 = static_cast<float>(pState->uiBackBufferWidth) * 10.f;
    //rhs->FrameRect.y2 = static_cast<float>(pState->uiBackBufferHeight) * 10.f;

    // copy onto task data.
    realp(pTaskData, rhs);

    // Scaleform_Render_Size<float> out;
    // GetRectSize(rhs->FrameRect, out);

    // auto fRect = rhs->FrameRect;
    // fRect.x2 = pState->uiBackBufferWidth;
    // fRect.y2 = pState->uiBackBufferHeight;

    // lhs->FrameRect = fRect;
    // rhs->FrameRect = fRect;
}

static TiltedPhoques::Initializer s_s([]() {
    TiltedPhoques::SwapCall(0x14102CC71, realp, &operatorEquals);

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

    Scaleform_GFx_MovieImpl_SetViewport =
        static_cast<decltype(Scaleform_GFx_MovieImpl_SetViewport)>((void*)(0x140F71A30));
    sub_140F67210 = static_cast<decltype(sub_140F67210)>((void*)(0x140F67210));

    CalculateSafeZoneInterface_X = static_cast<decltype(CalculateSafeZoneInterface_X)>((void*)(0x140F11310));
    CalculateSafeZoneInterface_Y = static_cast<decltype(CalculateSafeZoneInterface_Y)>((void*)(0x140F11360));
    Scaleform_GFx_MovieImpl_SetViewScaleMode =
        static_cast<decltype(Scaleform_GFx_MovieImpl_SetViewScaleMode)>((void*)(0x140F71A10));
});
