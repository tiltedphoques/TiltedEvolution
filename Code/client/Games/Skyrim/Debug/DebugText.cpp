
#include <Debug/DebugText.h>
#if 0

DebugText* (*DebugText_Instance)(bool) = nullptr;
void (*DebugText_DeleteAllText)(DebugText*) = nullptr;
void (*DebugText_Print)(DebugText*, const char*, float, float, int, int, float, int,
                                 const NiColorA*) = nullptr;

DebugText* DebugText::Instance(bool abCreate)
{
    return DebugText_Instance(abCreate);
}

void DebugText::DeleteAllDebugText()
{
    DebugText_DeleteAllText(this);
}

void DebugText::Print(const char* apText, float afX, float afY, int aiJustify, int aiLine_num, float afDie, int aiFont,
                      const NiColorA* apColor)
{
    DebugText_Print(this, apText, afX, afY, aiJustify, aiLine_num, afDie, aiFont, apColor);
}

#include <BSGraphics/BSGraphicsRenderer.h>
void DrawMyShit()
{
    auto* it = DebugText::Instance(true);
    it->DeleteAllDebugText();

    auto* pWindow = BSGraphics::GetMainWindow();

    NiColorA color{255.f, 255.f, 255.f, 255.f};
    //it->PrintLine(IDebugText::DEBUGTEXT_LOCATION::DTL_CENTER, IDebugText::DEBUGTEXT_JUSTIFY::DTJ_CENTER, "LOLOL");
    //it->Print("YEET", pWindow->uiWindowWidth / 2.f, pWindow->uiWindowHeight / 2.f, 2u, -1, -1.f, 0, &color);
    va_list list;
    it->VAPrintX(pWindow->uiWindowWidth / 2.f, DebugText::DEBUGTEXT_LOCATION::DTL_CENTER,
                 IDebugText::DEBUGTEXT_JUSTIFY::DTJ_CENTER, "sucko?", list, &color);
}

static void (*OldMainF)(void*);

static void MainF(void* apMain)
{
    DrawMyShit();
 
    OldMainF(apMain);
}

static void (*OldMainF222)(void*);

static void MainFFFFF(void* apMain)
{
    DrawMyShit();

    OldMainF222(apMain);
}

static TiltedPhoques::Initializer s_initDebugText([]() {
    const VersionDbPtr<void> debugTextInstance(82489);
    DebugText_Instance = static_cast<decltype(DebugText_Instance)>(debugTextInstance.GetPtr());

    const VersionDbPtr<void> printText(82508);
    DebugText_Print = static_cast<decltype(DebugText_Print)>(printText.GetPtr());

    const VersionDbPtr<void> deleteAll(82509);
    DebugText_DeleteAllText = static_cast<decltype(DebugText_DeleteAllText)>(deleteAll.GetPtr());

    TiltedPhoques::Put(DebugText_DeleteAllText, 0xC3);
    //TiltedPhoques::SwapCall(0x1405D9F67, OldMainF ,& MainF);

    TiltedPhoques::SwapCall(0x1405D9D84, OldMainF222, &MainFFFFF);
});
#endif
