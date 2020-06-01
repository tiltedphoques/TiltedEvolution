#if TP_SKYRIM64

#include <Games/Skyrim/DefaultObjectManager.h>

DefaultObjectManager& DefaultObjectManager::Get()
{
    using TGetDefaultObjectManager = DefaultObjectManager & ();

    POINTER_SKYRIMSE(TGetDefaultObjectManager, GetDefaultObjectManager, 0xF7210);

    return GetDefaultObjectManager();
}

#endif
