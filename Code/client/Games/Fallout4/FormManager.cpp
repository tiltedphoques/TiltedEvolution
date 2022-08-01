#include <TiltedOnlinePCH.h>

#include <FormManager.h>

FormManager* FormManager::Get()
{
    POINTER_FALLOUT4(FormManager*, s_instance, 711559);
    return *(s_instance.Get());
}
