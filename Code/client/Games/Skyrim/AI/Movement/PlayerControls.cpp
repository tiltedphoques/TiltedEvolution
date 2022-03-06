
#include "AI/Movement/PlayerControls.h"

PlayerControls* PlayerControls::GetInstance()
{
    POINTER_SKYRIMSE(PlayerControls*, s_instance, 400864);
    return *(s_instance.Get());
}

//static TiltedPhoques::Initializer s_Init([]() { TiltedPhoques::Put<uint8_t>(0x14072E490, 0xCC); });
