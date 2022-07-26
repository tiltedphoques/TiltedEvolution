#pragma once

#include <Actor.h>
#include <Misc/TintMask.h>
#include <Forms/ActorValueInfo.h>

struct Skills
{
    enum Skill : std::uint32_t
    {
        kOneHanded = 0,
        kTwoHanded = 1,
        kArchery = 2,
        kBlock = 3,
        kSmithing = 4,
        kHeavyArmor = 5,
        kLightArmor = 6,
        kPickpocket = 7,
        kLockpicking = 8,
        kSneak = 9,
        kAlchemy = 10,
        kSpeech = 11,
        kAlteration = 12,
        kConjuration = 13,
        kDestruction = 14,
        kIllusion = 15,
        kRestoration = 16,
        kEnchanting = 17,
        kTotal
    };

    static const Skill GetSkillFromActorValue(int32_t aActorValue) noexcept
    {
        switch (aActorValue)
        {
        case ActorValueInfo::kOneHanded:
            return kOneHanded;
        case ActorValueInfo::kTwoHanded:
            return kTwoHanded;
        case ActorValueInfo::kMarksman:
            return kArchery;
        case ActorValueInfo::kBlock:
            return kBlock;
        case ActorValueInfo::kSmithing:
            return kSmithing;
        case ActorValueInfo::kHeavyArmor:
            return kHeavyArmor;
        case ActorValueInfo::kLightArmor:
            return kLightArmor;
        case ActorValueInfo::kPickpocket:
            return kPickpocket;
        case ActorValueInfo::kLockpicking:
            return kLockpicking;
        case ActorValueInfo::kSneak:
            return kSneak;
        case ActorValueInfo::kAlchemy:
            return kAlchemy;
        case ActorValueInfo::kSpeechcraft:
            return kSpeech;
        case ActorValueInfo::kAlteration:
            return kAlteration;
        case ActorValueInfo::kConjuration:
            return kConjuration;
        case ActorValueInfo::kDestruction:
            return kDestruction;
        case ActorValueInfo::kIllusion:
            return kIllusion;
        case ActorValueInfo::kRestoration:
            return kRestoration;
        case ActorValueInfo::kEnchanting:
            return kEnchanting;
        default:
            return kTotal;
        }
    }

    static const char* GetSkillString(Skill aSkill) noexcept
    {
        switch (aSkill)
        {
        case kOneHanded:
            return "One-handed";
        case kTwoHanded:
            return "Two-handed";
        case kArchery:
            return "Archery";
        case kBlock:
            return "Block";
        case kSmithing:
            return "Smithing";
        case kHeavyArmor:
            return "Heavy armor";
        case kLightArmor:
            return "Light armor";
        case kPickpocket:
            return "Pickpocket";
        case kLockpicking:
            return "Lockpicking";
        case kSneak:
            return "Sneak";
        case kAlchemy:
            return "Alchemy";
        case kSpeech:
            return "Speech";
        case kAlteration:
            return "Alteration";
        case kConjuration:
            return "Conjuration";
        case kDestruction:
            return "Destruction";
        case kIllusion:
            return "Illusion";
        case kRestoration:
            return "Restoration";
        case kEnchanting:
            return "Enchanting";
        default:
            return "UNKNOWN";
        }
    }

    struct SkillData
    {
        float level;
        float xp;
        float levelThreshold;
    };
    static_assert(sizeof(SkillData) == 0xC);

    float xp;
    float levelThreshold;
    SkillData skills[Skill::kTotal];
    uint32_t legendaryLevels[Skill::kTotal];
};

struct TESQuest;

struct PlayerCharacter : Actor
{
    static constexpr FormType Type = FormType::Character;
    static int32_t LastUsedCombatSkill;

    static PlayerCharacter* Get() noexcept;

    static void SetGodMode(bool aSet) noexcept;

    const GameArray<TintMask*>& GetTints() const noexcept;

    // TODO: there's an in game function for this in fallout 4, maybe also for skyrim?
    void SetDifficulty(const int32_t aDifficulty, bool aForceUpdate = true, bool aExpectGameDataLoaded = true) noexcept;

    void AddSkillExperience(int32_t aSkill, float aExperience) noexcept;
    float GetSkillExperience(Skills::Skill aSkill) const noexcept
    {
        return (*pSkills)->skills[aSkill].xp;
    }

    NiPoint3 RespawnPlayer() noexcept;

    void PayCrimeGoldToAllFactions() noexcept;

    struct Objective
    {
        BSFixedString name;
        TESQuest* quest;
    };

    struct ObjectiveInstance
    {
        Objective* instance;
        uint64_t instanceCount;
    };

    uint8_t pad1[0x580 - sizeof(Actor)];
    GameArray<ObjectiveInstance> objectives; 
    uint8_t pad588[0x9B0 - 0x598];
    Skills** pSkills;
    uint8_t pad9B8[0xAC8 - 0x9B8];
    TESForm* locationForm;
    uint8_t padAC8[0x28];
    int32_t difficulty;
    uint8_t padAFC[0xB10 - 0xAFC];
    GameArray<TintMask*> baseTints;
    GameArray<TintMask*>* overlayTints;

    uint8_t padPlayerEnd[0xBE0 - 0xB30];
};

static_assert(offsetof(PlayerCharacter, objectives) == 0x580);
static_assert(offsetof(PlayerCharacter, pSkills) == 0x9B0);
static_assert(offsetof(PlayerCharacter, locationForm) == 0xAC8);
static_assert(offsetof(PlayerCharacter, baseTints) == 0xB10);
static_assert(offsetof(PlayerCharacter, overlayTints) == 0xB28);
static_assert(sizeof(PlayerCharacter) == 0xBE0);

