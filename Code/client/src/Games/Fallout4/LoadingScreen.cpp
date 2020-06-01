#if TP_FALLOUT4

#include <Games/Fallout4/LoadingScreen.h>
#include <Games/Fallout4/Misc/BSFixedString.h>

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
        apContext->value->data.m_data = str.m_data;

    RealRequestLoadingText(apContext);
}

static TiltedPhoques::Initializer s_loadingScreenHooks([]()
    {
        POINTER_FALLOUT4(TRequestLoadingText, s_requestLoadingText, 0x1412985D0 - 0x140000000);

        RealRequestLoadingText = s_requestLoadingText.Get();

        TP_HOOK(&RealRequestLoadingText, RequestLoadingText);
    });

#endif
