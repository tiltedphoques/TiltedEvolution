#include <TiltedOnlinePCH.h>

struct ScaleFormValue
{
    void* ptrs[5];
    uint32_t values[2];
    char* data; // 30 - 1C
};

struct ScaleFormReturn
{
    void* ptrs[7]; // again don't know if this is located here
    uint32_t values[6]; // again don't know if this is located here, this is used to make sure we land on the correct offsets of 32/64bits
    uint8_t someBool50; // 50 - 34
#if TP_PLATFORM_64
    uint8_t pad51[7];
#else
    uint8_t pad35[3];
#endif
    void* ptrs2[4];
    ScaleFormValue* scaleformValue; // 78 - 48
};

struct ScaleFormContext
{
    void** vtbl;
    void* somePointer; // not sure if located here, just know there is one
    uint32_t pad0[2];
    ScaleFormReturn* scaleformReturn; // 18 - 10
};


using TRequestLoadingText = void(__cdecl)(ScaleFormContext* apContext);
TRequestLoadingText* RealRequestLoadingText;

void RequestLoadingText(ScaleFormContext* apContext)
{
    static const char* s_displayStr = "TESSST";

    apContext->scaleformReturn->someBool50 = 0;
    if(apContext->scaleformReturn->scaleformValue)
        apContext->scaleformReturn->scaleformValue->data = (char*)s_displayStr;

    RealRequestLoadingText(apContext);
}

static TiltedPhoques::Initializer s_loadingScreenHooks([]()
    {
        POINTER_SKYRIMSE(TRequestLoadingText, s_requestLoadingText, 51925);

        RealRequestLoadingText = s_requestLoadingText.Get();

        //TP_HOOK(&RealRequestLoadingText, RequestLoadingText);
    });
