#pragma once

#include <Components/BaseFormComponent.h>

struct BGSVoiceType;
struct TESFaction;
struct TESLevItem;

struct TESActorBaseData : BaseFormComponent
{
    static uint32_t GetLeveledPickFormId(uint32_t aTempNpcFormId) noexcept;
    static void SetLeveledPickFormId(uint32_t aTempNpcFormId, uint32_t aTemplateFormId) noexcept;

    enum BaseFlags
    {
        IS_ESSENTIAL = 1 << 1,
    };

    uint32_t actorBaseFlags;
    int16_t magickaOffset;
    int16_t staminaOffset;
    uint16_t level;
    uint16_t calcLevelMin;
    uint16_t calcLevelMax;
    uint16_t speedMult;
    uint16_t baseDisposition;
    uint16_t templateUseFlags;
    int16_t healthOffset;
    int16_t bleedoutOverride;
    TESLevItem* deathItem;
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

    bool IsEssential() const noexcept { return actorBaseFlags & BaseFlags::IS_ESSENTIAL; }
    void SetEssential(bool aSet) noexcept
    {
        if (aSet)
            actorBaseFlags |= BaseFlags::IS_ESSENTIAL;
        else
            actorBaseFlags &= ~BaseFlags::IS_ESSENTIAL;
    }

    GameArray<FactionRank> factions;
};

static_assert(offsetof(TESActorBaseData, deathItem) == 0x20);
static_assert(offsetof(TESActorBaseData, baseTemplateForm) == 0x30);
static_assert(offsetof(TESActorBaseData, factions) == 0x40);
static_assert(sizeof(TESActorBaseData) == 0x58);
