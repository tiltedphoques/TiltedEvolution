#include <TiltedOnlinePCH.h>

#include <Components/TESTexture.h>

void TESTexture::Construct()
{
    TP_THIS_FUNCTION(TConstructor, TESTexture*, TESTexture);

    POINTER_SKYRIMSE(TConstructor, s_constructor, 0x1401AC180 - 0x140000000);

    ThisCall(s_constructor, this);
}
