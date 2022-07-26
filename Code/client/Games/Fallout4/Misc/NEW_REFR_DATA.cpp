#include <TiltedOnlinePCH.h>

#include <Misc/NEW_REFR_DATA.h>

NEW_REFR_DATA::NEW_REFR_DATA()
{
    POINTER_FALLOUT4(void*, s_vtbl, 1139813);
    // Set the vtable
    this->vtbl = s_vtbl.Get();

    unk40[0] = unk40[1] = unk40[2] = unk40[3] = 0;
    unk50[0] = unk50[1] = unk50[2] = unk50[3] = 0;

    unk60 = nullptr;
    unk68 = 0x1000000;
    randomFlag = 1;
}
