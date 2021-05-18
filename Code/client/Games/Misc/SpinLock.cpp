#include <Games/Misc/SpinLock.h>

void SpinLock::Lock() noexcept
{
    TP_THIS_FUNCTION(TLock, void, SpinLock, void*);

    POINTER_FALLOUT4(TLock, s_lock, 0x1400244D0 - 0x140000000);
    // TODO: WRONG ADDRESS!
    POINTER_SKYRIMSE(TLock, s_lock, 0x1400244D0 - 0x140000000);

    ThisCall(s_lock, this, nullptr);
}

void SpinLock::Unlock() noexcept
{
    TP_THIS_FUNCTION(TUnlock, void, SpinLock);

    POINTER_FALLOUT4(TUnlock, s_unlock, 0x140026AA0 - 0x140000000);
    // TODO: WRONG ADDRESS!
    POINTER_SKYRIMSE(TUnlock, s_unlock, 0x140026AA0 - 0x140000000);

    ThisCall(s_unlock, this);
}
