#pragma once

#include <Games/Primitives.h>
#include <Misc/BSString.h>
#include <Components/TESFullName.h>
#include <Forms/BGSStoryManagerTree.h>
#include <Components/TESCondition.h>

struct TESQuest;

struct BGSSceneAction
{
    enum class Type : uint32_t
    {
        kDialogue = 0,
        kPackage = 1,
        kTimer = 2,
        kPlayerDialogue = 3,
        kStartScene = 4,
        kNpcResponseDialogue = 5,
        kRadio = 6
    };

    virtual ~BGSSceneAction();
    virtual void sub_1(); // Load
    virtual void sub_2();
    virtual void sub_3();
    virtual void sub_4();
    virtual void sub_5();
    virtual bool Loops() const; // QIsLooping
    virtual void sub_7();
    virtual Type GetType() const;

    [[nodiscard]] uint32_t GetIndex() const noexcept { return index & 0x3FFFFFFF; }
    [[nodiscard]] bool IsRunning() const noexcept { return status & 1; }
    [[nodiscard]] bool IsComplete() const noexcept { return status & 2; }
    [[nodiscard]] bool IsInPhase(uint32_t aPhaseIndex) const noexcept { return aPhaseIndex >= startPhase && aPhaseIndex <= endPhase; }
    // Whether the phase waits for this action to complete before it ends
    [[nodiscard]] bool BlocksPhase(uint32_t aPhaseIndex) const noexcept
    {
        return IsInPhase(aPhaseIndex) && !IsInPhase(aPhaseIndex + 1) && IsRunning() && !IsComplete() && !Loops();
    }

    uint32_t actorId; // 0x08 quest alias ID
    uint16_t startPhase;
    uint16_t endPhase;
    uint32_t flags;
    uint8_t status; // 0x14, bit 0 = active, bit 1 = complete
    uint8_t unk15;
    uint16_t unk16;
    uint32_t index; // 0x18 unique ID in the low 30 bits
};

static_assert(offsetof(BGSSceneAction, actorId) == 0x8);
static_assert(offsetof(BGSSceneAction, startPhase) == 0xC);
static_assert(offsetof(BGSSceneAction, status) == 0x14);
static_assert(offsetof(BGSSceneAction, index) == 0x18);

struct BGSScenePhase
{
    TESCondition startConditions;
    // Once the phase has any, it ends as soon as they pass, even with actions still running
    TESCondition completionConditions;
    uint16_t phaseFlags;
    bool isRunning;

    [[nodiscard]] bool HasCompletionConditions() const noexcept { return completionConditions.pHead != nullptr; }
};

static_assert(offsetof(BGSScenePhase, completionConditions) == 0x8);

// Fallout 4 BGSScene: 0xE8.
struct BGSScene : TESForm
{
    // Same as Skyrim's: SetSceneActive moves currentPhaseIndex past the last phase on end, and resets it to -1 once the
    // end scripts ran
    [[nodiscard]] bool IsPlaying() const noexcept { return sceneFlags & 1; }
    [[nodiscard]] uint32_t GetNextPhaseIndex() const noexcept
    {
        if (!IsPlaying())
            return phases.length;

        return currentPhaseIndex == UINT32_MAX ? 0 : std::min(currentPhaseIndex, phases.length);
    }

    void* keywordFormBase;                // 0x20 IKeywordFormBase
    uint32_t sceneFlags;                  // 0x28 bit 0 = playing
    GameArray<BGSScenePhase*> phases;     // 0x30
    GameArray<uint32_t> actorIds;         // 0x48
    GameArray<uint32_t> actorFlags;       // 0x60
    GameArray<uint32_t> actorProgression; // 0x78
    GameArray<BGSSceneAction*> actions;   // 0x90
    TESQuest* owningQuest;                // 0xA8
    BGSScene* templateScene;              // 0xB0
    uint32_t flags;                       // 0xB8
    TESCondition conditions;              // 0xC0
    uint32_t speakerId;                   // 0xC8
    uint32_t currentPhaseIndex;           // 0xCC, -1 when not playing
    uint8_t padD0[0xE8 - 0xD0];
};
static_assert(offsetof(BGSScene, phases) == 0x30);
static_assert(offsetof(BGSScene, actorIds) == 0x48);
static_assert(offsetof(BGSScene, actions) == 0x90);
static_assert(offsetof(BGSScene, owningQuest) == 0xA8);
static_assert(offsetof(BGSScene, currentPhaseIndex) == 0xCC);
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
    // Returns whether the stage was reached, false when it waits for the quest to start
    bool ScriptSetStage(uint16_t stage);
    bool IsStageReached(uint16_t aStage) noexcept;
    // The stage's log entries, each sends TESQuestStageItemDoneEvent once its fragment ran
    uint32_t GetStageItemCount(uint16_t aStage) noexcept;
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
