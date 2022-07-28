#include <TiltedOnlinePCH.h>

#include <LoadingScreen.h>
#include <Misc/BSFixedString.h>

struct ScaleFormValue
{
    uint8_t pad0[0x14];
    uint32_t unk14;
    uint8_t pad18[0x30 - 0x18];
    BSFixedString data;
};

struct ScaleFormContext
{
    uint8_t pad0[0xE8];
    ScaleFormValue* value;
    uint8_t padF0[0x270 - 0xF0];
    uint8_t isSet;
};

using TRequestLoadingText = void(__fastcall)(ScaleFormContext* apContext);
TRequestLoadingText* RealRequestLoadingText;
void RequestLoadingText(ScaleFormContext* apContext)
{
    TP_EMPTY_HOOK_PLACEHOLDER;

    BSFixedString str("TESTTTTT");

    if(apContext && apContext->value)
        apContext->value->data.data = str.data;

    RealRequestLoadingText(apContext);
}

static TiltedPhoques::Initializer s_loadingScreenHooks([]()
    {
        POINTER_FALLOUT4(TRequestLoadingText, s_requestLoadingText, 277977);

        RealRequestLoadingText = s_requestLoadingText.Get();

        TP_HOOK(&RealRequestLoadingText, RequestLoadingText);
    });

