#pragma once

#include <Actor.h>

struct TESQuest;
struct TintMask;

struct PlayerCharacter : Actor
{
    static constexpr FormType Type = FormType::Character;
    static int32_t LastUsedCombatSkill;

    static PlayerCharacter* Get() noexcept;

    static void SetGodMode(bool aSet) noexcept;

    virtual void sub_133();
    virtual void sub_134();
    virtual void sub_135();
    virtual void sub_136();

    const GameArray<TintMask*>& GetTints() const noexcept;

    void SetDifficulty(const int32_t aDifficulty, bool aForceUpdate = true, bool aExpectGameDataLoaded = true) noexcept;

    void AddSkillExperience(int32_t aSkill, float aExperience) noexcept;

    NiPoint3 RespawnPlayer() noexcept;

    void PayCrimeGoldToAllFactions() noexcept;

    void SetWaypoint(NiPoint3* apPosition, TESWorldSpace* apWorldSpace) noexcept;
    void RemoveWaypoint() noexcept;

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

    // FO4 PlayerCharacter: 0xB58. objectives at 0x7D8 per libxse.
    uint8_t pad1[0x7D8 - sizeof(Actor)];
    GameArray<ObjectiveInstance> objectives; // 0x7D8
    uint8_t pad588[0xB60 - 0x7F0];
};

static_assert(offsetof(PlayerCharacter, objectives) == 0x7D8);
static_assert(sizeof(PlayerCharacter) == 0xB60);
