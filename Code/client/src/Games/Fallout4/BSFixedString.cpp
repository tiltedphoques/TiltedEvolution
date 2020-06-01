#if TP_FALLOUT4

#include <Games/Fallout4/Misc/BSFixedString.h>

BSFixedString::BSFixedString(const char* acpData)
{
    TP_THIS_FUNCTION(TConstructor, void, BSFixedString, const char*);

    POINTER_FALLOUT4(TConstructor, s_constructor, 0x141B41D40 - 0x140000000);

    ThisCall(s_constructor, this, acpData);
}

void BSFixedString::Release()
{
    TP_THIS_FUNCTION(TConstructor, void, BSFixedString);

    POINTER_FALLOUT4(TConstructor, s_release, 0x141B42FD0 - 0x140000000);

    ThisCall(s_release, this);
}

BSFixedString::~BSFixedString()
{
    // Release();
}

#endif
