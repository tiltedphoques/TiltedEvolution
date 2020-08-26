#pragma once

#if TP_FALLOUT

#include <Games/Primitives.h>
#include <Games/Fallout4/Forms/BGSStoryManagerTree.h>
#include <Games/Fallout4/Misc/BSString.h>
#include <Games/Fallout4/Misc/BSFixedString.h>

struct TESQuest : BGSStoryManagerTreeForm
{
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

        inline bool IsDone()
        {
            return flags & 1;
        }
    };

    void* pUnkQuest;         // maybe a TESFullname?
    char pad30[0xC4];        // 0x0030
    uint16_t flags;          // 0x00F4
    uint8_t priority;        // 0x00F6
    uint8_t type;            // 0x00F7
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

    inline bool IsActive() const { return ((flags >> 11) & 1); }
    inline bool IsCompleted() const { return ((flags >> 1) & 1); }
    inline bool IsStopped() const { return !(flags & Flags::Enabled) && flags >= 0; }

    bool UnkSetRunning(bool& succeded, bool force);
};

static_assert(sizeof(TESQuest) == 752);
//static_assert(offsetof(TESQuest, fullName) == 0x28);
static_assert(offsetof(TESQuest, objectives) == 0x118);
static_assert(offsetof(TESQuest, currentStage) == 0x2B4);
static_assert(offsetof(TESQuest, flags) == 0xF4);
static_assert(offsetof(TESQuest, idName) == 0x2B8);

#endif
