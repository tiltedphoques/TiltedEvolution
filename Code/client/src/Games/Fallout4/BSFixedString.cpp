#if TP_FALLOUT4

#include <Games/Fallout4/Misc/BSFixedString.h>

BSFixedString::BSFixedString(const char* acpData)
{
    TP_THIS_FUNCTION(TConstructor, void, BSFixedString, const char*);

    POINTER_FALLOUT4(TConstructor, s_constructor, 0x141B41D40 - 0x140000000);

    ThisCall(s_constructor, this, acpData);
}

BSFixedString::BSFixedString(BSFixedString&& aRhs)
{
    m_data = aRhs.m_data;
    aRhs.m_data = nullptr;
}

BSFixedString& BSFixedString::operator=(BSFixedString&& aRhs)
{
    Release();

    m_data = aRhs.m_data;
    aRhs.m_data = nullptr;

    return *this;
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
