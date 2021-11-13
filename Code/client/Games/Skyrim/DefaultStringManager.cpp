#include <TiltedOnlinePCH.h>

#include <DefaultStringManager.h>

DefaultStringManager& DefaultStringManager::Get()
{
    using TGetDefaultStringManager = DefaultStringManager & ();

    POINTER_SKYRIMSE(TGetDefaultStringManager, GetDefaultStringManager, 0x14010EA60 - 0x140000000);

    return GetDefaultStringManager();
}
