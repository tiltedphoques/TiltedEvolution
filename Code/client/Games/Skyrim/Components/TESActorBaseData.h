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
    TESForm* baseTemplateForm;
    uint32_t changeFlags;
    uint32_t pad3C;

    struct FactionRank
    {
        TESFaction* faction;
        int8_t rank;
        uint8_t pad09{ 0 };
        uint16_t pad0A{ 0 };
        uint32_t pad0C{ 0 };
    };
    static_assert(sizeof(FactionRank) == 0x10);

    bool IsEssential() const noexcept { return flags & BaseFlags::IS_ESSENTIAL; }
    void SetEssential(bool aSet) noexcept
    {
        if (aSet)
            flags |= BaseFlags::IS_ESSENTIAL;
        else
            flags &= ~BaseFlags::IS_ESSENTIAL;
    }

    GameArray<FactionRank> factions;
};

static_assert(offsetof(TESActorBaseData, baseTemplateForm) == 0x30);
static_assert(offsetof(TESActorBaseData, factions) == 0x40);
