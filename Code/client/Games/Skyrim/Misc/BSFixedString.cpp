#include <TiltedOnlinePCH.h>

#include <Misc/BSFixedString.h>

BSFixedString::BSFixedString()
    : data{nullptr}
{
    
}

BSFixedString::BSFixedString(const char* acpData)
{
    TP_THIS_FUNCTION(TConstructor, void, BSFixedString, const char*);

    POINTER_SKYRIMSE(TConstructor, s_constructor, 0x140C4E840 - 0x140000000);

    ThisCall(s_constructor, this, acpData);
}

BSFixedString::~BSFixedString()
{
    // Release();
}

void BSFixedString::Set(const char* acpData)
{
    TP_THIS_FUNCTION(TSet, void, BSFixedString, const char*);

    POINTER_SKYRIMSE(TSet, s_set, 0x140C4E9D0 - 0x140000000);

    ThisCall(s_set, this, acpData);
}

void BSFixedString::Release() noexcept
{
    TP_THIS_FUNCTION(TRelease, void, BSFixedString);

    POINTER_SKYRIMSE(TRelease, s_release, 0x140C4FC10 - 0x140000000);

    ThisCall(s_release, this);
}
