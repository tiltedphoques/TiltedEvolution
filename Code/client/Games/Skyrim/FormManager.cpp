#include <TiltedOnlinePCH.h>

#include <FormManager.h>

// TESDataHandler
FormManager* FormManager::Get()
{
    POINTER_SKYRIMSE(FormManager*, s_instance, 400269);
    return *(s_instance.Get());
}
