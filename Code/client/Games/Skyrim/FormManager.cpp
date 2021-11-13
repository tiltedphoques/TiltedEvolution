#include <TiltedOnlinePCH.h>

#include <FormManager.h>

FormManager* FormManager::Get()
{
    POINTER_SKYRIMSE(FormManager*, s_instance, 0x141F58C28 - 0x140000000);
    return *(s_instance.Get());
}
