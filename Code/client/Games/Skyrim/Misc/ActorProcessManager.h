#pragma once

struct TESForm;
struct MiddleProcess;

struct ActorProcessManager
{
    enum HEAD_TRACK_TYPE
    {
        HEAD_TRACK_TYPE_DEFAULT = 0,
        HEAD_TRACK_TYPE_UNK1,
        HEAD_TRACK_TYPE_SCRIPT,
        HEAD_TRACK_TYPE_UNK3,
        HEAD_TRACK_TYPE_DIALOG,
        HEAD_TRACK_TYPE_UNK5,
    };

    void* unk0;
    MiddleProcess* middleProcess;
    void* unk8;
    struct TESPackage* package;
    uint32_t unk10[9];
    uint32_t unk34[8];
    float unk54;
    uint32_t unk58[4];
    TESForm* equippedObject[2];

    uint8_t pad88[0x137 - 0x88];

    int8_t movementType;
};

static_assert(offsetof(ActorProcessManager, movementType) == 0x137);
