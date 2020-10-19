#include <DefaultStringManager.h>

DefaultStringManager& DefaultStringManager::Get()
{
    using TGetDefaultStringManager = DefaultStringManager & ();

    POINTER_SKYRIMSE(TGetDefaultStringManager, GetDefaultStringManager, 0x104AD0);

    return GetDefaultStringManager();
}
