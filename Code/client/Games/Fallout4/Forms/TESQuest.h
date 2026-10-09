#pragma once

#include <Games/Primitives.h>
#include <Misc/BSString.h>
#include <Components/TESFullName.h>
#include <Forms/BGSStoryManagerTree.h>

// Fallout 4 BGSScene: 0xE8.
struct BGSScene : TESForm
{
    uint8_t pad20[0x30 - 0x20];
    GameArray<void*> phases;      // 0x30
    GameArray<uint32_t> actorIds; // 0x48
    uint8_t pad60[0xE8 - 0x60];
};
static_assert(offsetof(BGSScene, actorIds) == 0x48);
static_assert(sizeof(BGSScene) == 0xE8);

// Fallout 4 TESQuest: 0x2F0.
struct TESQuest : BGSStoryManagerTreeForm
{
    enum class State : uint8_t
    {
        WaitingPromotion,
        Running,
        Stopped,
        WaitingForStage
    };

    enum Flags : uint16_t
    {
        StopStart = USHRT_MAX,
        None = 0,
        Enabled = 1 << 0,
        Completed = 1 << 1,
        AddIdleToHello = 1 << 2,
        AllowRepeatStages = 1 << 3,
        StartsEnabled = 1 << 4,
        DisplayedInHUD = 1 << 5,
        Failed = 1 << 6,
        StageWait = 1 << 7,
        RunOnce = 1 << 8,
        ExcludeFromExport = 1 << 9,
        WarnOnAliasFillFailure = 1 << 10,
        Active = 1 << 11,
        RepeatsConditions = 1 << 12,
        KeepInstance = 1 << 13,
        WantDormant = 1 << 14,
        HasDialogueData = 1 << 15
    };

    enum class Type : uint8_t
    {
        None = 0,
        MainQuest = 1,
        BrotherhoodOfSteel = 2,
        Institute = 3,
        Minutemen = 4,
        Railroad = 5,
        Miscellaneous = 6,
        SideQuest = 7,
    };

    // TESQuestStage
    struct Stage
    {
        GameArray<void*> items; // 0x00
        uint16_t stageIndex;    // 0x18
        uint8_t flags;          // 0x1A

        inline bool IsDone() { return flags & 1; }
    };
    static_assert(offsetof(Stage, stageIndex) == 0x18);

    TESFullName fullName;          // 0x28
    uint8_t pad38[0x70 - 0x38];
    GameArray<void*> aliases;      // 0x70
    uint8_t pad88[0xF0 - 0x88];
    float questDelay;              // 0xF0 QUEST_DATA
    uint16_t flags;                // 0xF4
    uint8_t priority;              // 0xF6
    Type type;                     // 0xF7
    uint32_t eventID;              // 0xF8
    GameArray<Stage*> stages;      // 0x100
    GameArray<void*> objectives;   // 0x118
    uint8_t pad130[0x290 - 0x130];
    GameArray<BGSScene*> scenes;   // 0x290
    uint8_t pad2A8[0x2B4 - 0x2A8];
    uint16_t currentStage;         // 0x2B4
    bool alreadyRun;               // 0x2B6
    BSString idName;               // 0x2B8
    void* pStartEventData;         // 0x2C8
    uint8_t pad2D0[0x2F0 - 0x2D0];

    TESObjectREFR* GetAliasedRef(uint32_t aiAliasID) noexcept;

    bool IsStageDone(uint16_t stageIndex);

    void SetActive(bool toggle);

    inline bool IsEnabled() const { return flags & Flags::Enabled; }
    inline bool IsActive() const { return flags & Flags::Active; }
    inline bool IsStopped() const { return (flags & (Flags::Enabled | Flags::StageWait)) == 0; }

    bool EnsureQuestStarted(bool& aStartDelayed, bool aImmediate);

    bool SetStage(uint16_t stage);
    void ScriptSetStage(uint16_t stage);
    // Applies a stage that was requested while the quest was still starting.
    static void ApplyPendingStage(uint32_t aFormId) noexcept;
    void SetStopped();
};

static_assert(offsetof(TESQuest, fullName) == 0x28);
static_assert(offsetof(TESQuest, flags) == 0xF4);
static_assert(offsetof(TESQuest, type) == 0xF7);
static_assert(offsetof(TESQuest, stages) == 0x100);
static_assert(offsetof(TESQuest, scenes) == 0x290);
static_assert(offsetof(TESQuest, currentStage) == 0x2B4);
static_assert(offsetof(TESQuest, idName) == 0x2B8);
static_assert(sizeof(TESQuest) == 0x2F0);
