#include <TiltedOnlinePCH.h>

#include <DefaultObjectManager.h>

DefaultObjectManager& DefaultObjectManager::Get()
{
    using TGetDefaultObjectManager = DefaultObjectManager & ();

    POINTER_SKYRIMSE(TGetDefaultObjectManager, GetDefaultObjectManager, 0xF7210);

    return GetDefaultObjectManager();
}
