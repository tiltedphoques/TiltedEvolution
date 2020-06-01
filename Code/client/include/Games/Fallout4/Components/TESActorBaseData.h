#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/Components/BaseFormComponent.h>

struct BGSVoiceType;

struct TESActorBaseData : BaseFormComponent
{
    virtual ~TESActorBaseData();
    virtual void sub_7();
    virtual void sub_8();
    virtual void sub_9();

    uint64_t flags;
    uint8_t unk10[0x28 - 0x10];
    BGSVoiceType* voiceType;
    uint8_t unk30[0x68 - 0x30];
};

static_assert(sizeof(TESActorBaseData) == 0x68);

#endif
