#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Components/BaseFormComponent.h>
#include <Games/Skyrim/Misc/BSFixedString.h>

struct TESModel : BaseFormComponent
{
    BSFixedString name;
    void* unk8[2];
    uint8_t unk10[4];
};

#endif
