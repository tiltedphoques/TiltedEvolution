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

    struct alignas(sizeof(void*)) FactionInfo
    {
        struct TESFaction* faction;
        int8_t rank;
    };

    uint64_t flags;
    uint8_t unk10[0x28 - 0x10];
    BGSVoiceType* voiceType;
    uint8_t unk30[0x50 - 0x30];
    GameArray<FactionInfo> factions;
};

static_assert(sizeof(TESActorBaseData) == 0x68);
static_assert(offsetof(TESActorBaseData, factions) == 0x50);

#endif
