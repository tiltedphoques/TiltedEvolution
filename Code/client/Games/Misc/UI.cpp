#include <TiltedOnlinePCH.h>

#include <Games/Misc/UI.h>
#include <Misc/BSFixedString.h>

UI* UI::Get() noexcept
{
    POINTER_SKYRIMSE(UI*, s_instance, 0x141F59320 - 0x140000000);
    POINTER_FALLOUT4(UI*, s_instance, 0x1458D0898 - 0x140000000);

    return *s_instance.Get();
}

bool UI::IsOpen(const BSFixedString& acName) const noexcept
{
    if (acName.data == nullptr)
        return false;

    TP_THIS_FUNCTION(TMenuSystem_IsOpen, bool, const UI, const BSFixedString&);

    POINTER_SKYRIMSE(TMenuSystem_IsOpen, s_isMenuOpen, 0x140F043C0 - 0x140000000);
    POINTER_FALLOUT4(TMenuSystem_IsOpen, s_isMenuOpen, 0x142042160 - 0x140000000);

    return ThisCall(s_isMenuOpen.Get(), this, acName);
}
