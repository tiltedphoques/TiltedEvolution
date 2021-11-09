#include <TiltedOnlinePCH.h>

#include <DefaultObjectManager.h>

DefaultObjectManager& DefaultObjectManager::Get()
{
    using TGetDefaultObjectManager = DefaultObjectManager & ();

    POINTER_SKYRIMSE(TGetDefaultObjectManager, GetDefaultObjectManager, 0x1400F7210 - 0x140000000);

    return GetDefaultObjectManager();
}
