#include <FormManager.h>

FormManager* FormManager::Get()
{
    POINTER_FALLOUT4(FormManager*, s_instance, 0x1458CF080 - 0x140000000);
    return *(s_instance.Get());
}
