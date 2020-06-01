#include <Games/TES.h>

TES* TES::Get() noexcept
{
    POINTER_SKYRIMSE(TES*, tes, 0x141EC0A50 - 0x140000000);
    POINTER_FALLOUT4(TES*, tes, 0x145AA4288 - 0x140000000);

    return *tes.Get();
}

