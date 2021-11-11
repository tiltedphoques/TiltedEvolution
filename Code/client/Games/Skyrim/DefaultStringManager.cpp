#include <TiltedOnlinePCH.h>

#include <DefaultStringManager.h>

DefaultStringManager& DefaultStringManager::Get()
{
    using TGetDefaultStringManager = DefaultStringManager & ();

    POINTER_SKYRIMSE(TGetDefaultStringManager, GetDefaultStringManager, 0x140104AD0 - 0x140000000);

    return GetDefaultStringManager();
}
