#pragma once

#include <Components/BaseFormComponent.h>

struct BGSVoiceType;
struct TESFaction;

struct TESActorBaseData : BaseFormComponent
{
    enum BaseFlags
    {
        IS_ESSENTIAL = 1 << 1,
    };

    uint32_t flags;
    uint16_t unk08;
    uint16_t unk0A;
    uint16_t level;
    uint16_t minLevel;
    uint16_t maxLevel;
    uint16_t unk12;
    uint16_t unk14;
    uint16_t unk16;
    uint16_t unk18;
    uint16_t unk1A;
    void* unk1C;
    BGSVoiceType* voiceType;
    TESForm* owner;
    uint32_t unk28;

    struct alignas(sizeof(void*)) FactionInfo
    {
        TESFaction* faction;
        int8_t rank;
    };

    bool IsEssential() const noexcept
    {
        return flags & BaseFlags::IS_ESSENTIAL;
    }
    void SetEssential(bool aSet) noexcept
    {
        if (aSet)
            flags |= BaseFlags::IS_ESSENTIAL;
        else
            flags &= ~BaseFlags::IS_ESSENTIAL;
    }

    GameArray<FactionInfo> factions;
};

static_assert(offsetof(TESActorBaseData, owner) == 0x30);
static_assert(offsetof(TESActorBaseData, factions) == 0x40);
