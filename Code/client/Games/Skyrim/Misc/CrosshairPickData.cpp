#include <TiltedOnlinePCH.h>

#include <Misc/CrosshairPickData.h>

CrosshairPickData* CrosshairPickData::Get()
{
    POINTER_SKYRIMSE(CrosshairPickData*, s_instance, 401585);

    return *s_instance.Get();
}
