#pragma once

#include <Games/Primitives.h>
#include <Misc/BSString.h>
#include <Components/TESFullName.h>
#include <Forms/BGSStoryManagerTree.h>

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
        Disabled,
        Enabled = 1 << 0,
        Completed = 1 << 1,
        Started = 1 << 7,
        Unk = 2 << 10, // this is very likely a combination
        Unk2 = 8 << 10,
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
        uint16_t flags;

        // Below looks very wrong. Likely a list
        char pad4[16];
        uint16_t flags2;

        inline bool IsDone() { return flags & 1; }
    };

    TESFullName fullName;
    char pad30[0x20];              // 0x0030
    GameArray<void*> aliases;      // 0x0058
    char pad70[0x6C];              // 0x0070
    uint16_t flags;                // 0x00DC default init: 256
    uint8_t priority;              // 0x00DE
    uint8_t type;                  // 0x00DF
    int32_t scopedStatus;          // 0x00E0 default init: -1, if not -1 outside of story manager scope
    uint32_t padE4;
    GameList<Stage> stages;            // 0x00E8
    GameList<Objective> objectives;    // 0x00F8
    char pad108[0x100];                // 0x0108
    GameArray<uint64_t> N000008BE;     // 0x0208
    char pad210[8];                    // 0x0210
    uint16_t currentStage;             // 0x0228 
    char pad228[2];
    BSString idName; // < this is the proper quest ID
    uint64_t pad;
    uint64_t unkFlags;
    char pad250[24];

    bool IsStageDone(uint16_t stageIndex);
    void SetCompleted(bool force);

    void CompleteAllObjectives();// completes all objectives + stages
    void SetActive(bool toggle); // < is the quest selected in journal and followed?

    inline bool IsEnabled() const { return flags & Flags::Enabled; }
    inline void Disable() { flags &= ~Flags::Enabled; };

    inline bool IsActive() const { return flags & 0x800; }
    inline bool IsStopped() const { return (flags & (Flags::Enabled | Flags::Started)) == 0; } // & 0x81

    bool Kill();
    State getState();

    bool UnkSetRunning(bool &succeded, bool force);

    bool SetStage(uint16_t stage);
    void SetStopped();
};

static_assert(sizeof(TESQuest) == 616);
static_assert(offsetof(TESQuest, fullName) == 0x28);
static_assert(offsetof(TESQuest, objectives) == 0xF8);
static_assert(offsetof(TESQuest, currentStage) == 0x228);
static_assert(offsetof(TESQuest, unkFlags) == 0x248);
static_assert(offsetof(TESQuest, flags) == 0xDC);
