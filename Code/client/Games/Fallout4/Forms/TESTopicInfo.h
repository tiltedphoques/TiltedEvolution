#pragma once

#include <Forms/TESForm.h>

struct TESTopicInfo : TESForm
{
    uint8_t pad20[0x42 - 0x20];
    uint8_t ucSubtitlePriority;
    uint8_t pad43[0x50 - 0x43];
};

static_assert(sizeof(TESTopicInfo) == 0x50);
