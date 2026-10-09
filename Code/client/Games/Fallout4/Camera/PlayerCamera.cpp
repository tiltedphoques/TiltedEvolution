#include <TiltedOnlinePCH.h>

#include <Camera/PlayerCamera.h>

PlayerCamera* PlayerCamera::Get() noexcept
{
    static VersionDbPtr<PlayerCamera*> s_instance(4796065);
    return *s_instance.Get();
}

bool PlayerCamera::IsFirstPerson() noexcept
{
    TP_THIS_FUNCTION(TCameraEquals, bool, PlayerCamera, int32_t);
    static VersionDbPtr<TCameraEquals> cameraEquals(2248421);
    return TiltedPhoques::ThisCall(cameraEquals, this, 0);
}
