#pragma once

#include <Games/Primitives.h>
#include <Forms/BGSStoryManagerTree.h>
#include <Misc/BSString.h>
#include <Misc/BSFixedString.h>

struct TESQuest : BGSStoryManagerTreeForm
{
    enum Flags : uint16_t
    {
        Disabled,
        Enabled = 1 << 0,
        Completed = 1 << 1,
        StageWait = 1 << 7,
        Unk = 2 << 10, // this is very likely a combination
        Unk2 = 8 << 10,
    };

    enum class Type : uint8_t
    {
        None = 0x0,
        QTYPE_MAINQUEST = 0x1,
        QTYPE_BROTHERHOOD = 0x2,
        QTYPE_INSTITUTE = 0x3,
        QTYPE_MINUTEMEN = 0x4,
        QTYPE_RAILROAD = 0x5,
        Miscellaneous = 0x6,
        QTYPE_SIDEQUESTS = 0x7,
        QTYPE_DLC01 = 0x8,
        QTYPE_DLC02 = 0x9,
        QTYPE_DLC03 = 0xA,
        QTYPE_COUNT = 0xB,
    };


    struct Objective
    {
        BSFixedString nameRef;
        TESQuest* parent;
        char pad10[0x8];       // 0x0010
        uint16_t flags;        // 0x0018
        char pad1A[0x2];       // 0x001A
        uint16_t stageId;      // 0x001C
    };

    struct Stage
    {
        uint32_t pad0;
        char pad4[0x4];           // 0x0004
        uint16_t stageIndex;      // 0x0008
        char padA[0x6];           // 0x000A
        int32_t someArrayCount;   // 0x0010
        char pad14[0x6];          // 0x0014
        uint8_t flags;            // 0x001A
        uint16_t pad1B;           // 0x001B

        inline bool IsDone() const
        {
            return flags & 1;
        }
    };

    void* pUnkQuest;         // maybe a TESFullname?
    char pad30[0xC4];        // 0x0030
    uint16_t flags;          // 0x00F4
    uint8_t priority;        // 0x00F6
    Type  type;            // 0x00F7
    int32_t scopedStatus;    // 0x00F8
    char padFC[0x4];         // 0x00FC
    GameArray<Stage> stages; // 0x0100
    GameArray<Objective> objectives; // 0x0118
    char pad130[0x184];              // 0x0130
    uint16_t currentStage;           // 0x02B4
    uint16_t pad2B6;                 // 0x02B6
    BSString idName;                 // 0x02B8
    char pad2C4[0x2C- 8];

    void SetActive(bool toggle);
    bool SetStage(uint16_t stage);
    void SetStopped();
    void ScriptSetStage(uint16_t stage);

    inline bool IsActive() const { return ((flags >> 11) & 1); }
    inline bool IsCompleted() const { return ((flags >> 1) & 1); }
    inline bool IsStopped() const { return !(flags & Flags::Enabled) && flags >= 0; }

    bool EnsureQuestStarted(bool& succeded, bool force);
};

static_assert(sizeof(TESQuest) == 752);
//static_assert(offsetof(TESQuest, fullName) == 0x28);
static_assert(offsetof(TESQuest, objectives) == 0x118);
static_assert(offsetof(TESQuest, currentStage) == 0x2B4);
static_assert(offsetof(TESQuest, flags) == 0xF4);
static_assert(offsetof(TESQuest, idName) == 0x2B8);
