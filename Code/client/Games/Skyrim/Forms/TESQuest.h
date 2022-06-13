#pragma once

#include <Games/Primitives.h>
#include <Misc/BSString.h>
#include <Components/TESFullName.h>
#include <Forms/BGSStoryManagerTree.h>

struct BGSScene : TESForm
{
    GameArray<void*> phases;
    GameArray<uint32_t> actorIds;
};

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
        MagesGuild = 2,
        ThievesGuild = 3,
        DarkBrotherhood = 4,
        CompanionsQuest = 5,
        Miscellaneous = 6,
        Daedric = 7,
        SideQuest = 8,
        CivilWar = 9,
        DLC01_Vampire = 10,
        DLC02_Dragonborn = 11
    };

    struct Objective
    {
        BSFixedString nameRef; // 0x0000
        TESQuest* parent;      // 0x0008
        char pad10[12];       // 0x0010
        uint16_t stageId;
        uint8_t pad30;
        uint8_t state;
    };

    static_assert(offsetof(Objective, state) == 31);

    struct Stage
    {
        uint16_t stageIndex;
        uint8_t flags;

        inline bool IsDone() { return flags & 1; }
    };

    TESFullName fullName;
    GameArray<void*> instanceData;
    uint32_t currentInstanceID;
    GameArray<void*> aliases;          // 0x0058
    char pad70[0xD8 - 0x70];
    float questDelay;                  // 0x00D8
    uint16_t flags;                    // 0x00DC default init: 256
    uint8_t priority;                  // 0x00DE
    Type type;                         // 0x00DF
    int32_t scopedStatus;              // 0x00E0 default init: -1, if not -1 outside of story manager scope
    uint32_t padE4;
    GameList<Stage> stages;
    /*
    GameList<Stage>* pExecutedStages;  // 0x00E8
    GameList<Stage>* pWaitingStages;   // 0x00F0
    */
    GameList<Objective> objectives;    // 0x00F8
    char pad108[0x100];                // 0x0108
    GameArray<BGSScene*> scenes;       // 0x0208
    char pad210[8];                    // 0x0210
    uint16_t currentStage;             // 0x0228 
    bool alreadyRun;                   // 0x022A
    char pad22B[2];
    BSString idName; // < this is the proper quest ID
    void* pStartEventData;
    uint64_t unkFlags;
    char pad250[24];

    TESObjectREFR* GetAliasedRef(uint32_t aiAliasID) noexcept;

    bool IsStageDone(uint16_t stageIndex);
    void SetCompleted(bool force);

    void CompleteAllObjectives();// completes all objectives + stages
    void SetActive(bool toggle); // < is the quest selected in journal and followed?

    inline void Disable() { flags &= ~Flags::Enabled; };

    inline bool IsEnabled() const { return flags & Flags::Enabled; }
    inline bool IsActive() const { return flags & Flags::Active; }
    inline bool IsStopped() const { return (flags & (Flags::Enabled | Flags::StageWait)) == 0; } // & 0x81

    bool Kill();
    State getState();

    bool EnsureQuestStarted(bool &succeded, bool force);

    bool SetStage(uint16_t stage);
    void ScriptSetStage(uint16_t stage);
    void SetStopped();
};

static_assert(sizeof(TESQuest) == 0x268);
static_assert(offsetof(TESQuest, fullName) == 0x28);
static_assert(offsetof(TESQuest, flags) == 0xDC);
static_assert(offsetof(TESQuest, stages) == 0xE8);
static_assert(offsetof(TESQuest, objectives) == 0xF8);
static_assert(offsetof(TESQuest, currentStage) == 0x228);
static_assert(offsetof(TESQuest, unkFlags) == 0x248);
