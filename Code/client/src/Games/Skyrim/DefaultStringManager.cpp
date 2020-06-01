#if TP_SKYRIM64

#include <Games/Skyrim/DefaultStringManager.h>

DefaultStringManager& DefaultStringManager::Get()
{
    using TGetDefaultStringManager = DefaultStringManager & ();

    POINTER_SKYRIMSE(TGetDefaultStringManager, GetDefaultStringManager, 0x104AD0);

    return GetDefaultStringManager();
}

#endif
