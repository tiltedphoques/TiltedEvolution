#include <TiltedOnlinePCH.h>

#include <Misc/BSFixedString.h>

BSFixedString::BSFixedString(const char* acpData)
{
    TP_THIS_FUNCTION(TConstructor, void, BSFixedString, const char*);

    POINTER_FALLOUT4(TConstructor, s_constructor, 640274);

    ThisCall(s_constructor, this, acpData);
}

BSFixedString::BSFixedString(BSFixedString&& aRhs) noexcept
{
    data = aRhs.data;
    aRhs.data = nullptr;
}

BSFixedString& BSFixedString::operator=(BSFixedString&& aRhs) noexcept
{
    Release();

    data = aRhs.data;
    aRhs.data = nullptr;

    return *this;
}


void BSFixedString::Release() noexcept
{
    TP_THIS_FUNCTION(TConstructor, void, BSFixedString);

    POINTER_FALLOUT4(TConstructor, s_release, 1204431);

    ThisCall(s_release, this);
}

void BSFixedString::Set(const char* acpStr) noexcept
{
    TP_THIS_FUNCTION(TSet, void, BSFixedString, const char* acpStr);

    POINTER_FALLOUT4(TSet, s_set, 1064964);

    ThisCall(s_set, this, acpStr);
}

BSFixedString::~BSFixedString()
{
    // Release();
}
