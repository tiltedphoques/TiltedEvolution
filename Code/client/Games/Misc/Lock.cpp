#include <Games/Misc/Lock.h>

void Lock::SetLock(bool abIsLocked) noexcept
{
    TP_THIS_FUNCTION(TSetLock, void, Lock, bool);
    POINTER_SKYRIMSE(TSetLock, realSetLock, 0x140134AF0 - 0x140000000);
    POINTER_FALLOUT4(TSetLock, realSetLock, 0x1400B7E10 - 0x140000000);

    return ThisCall(realSetLock, this, abIsLocked);
}
