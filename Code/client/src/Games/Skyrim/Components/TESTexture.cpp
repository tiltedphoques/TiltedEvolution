#include <Games/Skyrim/Components/TESTexture.h>

#if TP_SKYRIM

void TESTexture::Construct()
{
    TP_THIS_FUNCTION(TConstructor, TESTexture*, TESTexture);

    POINTER_SKYRIMSE(TConstructor, s_constructor, 0x1401A0BC0 - 0x140000000);

    ThisCall(s_constructor, this);
}

#endif
