#pragma once

struct TESForm;
struct MiddleProcess;
struct TESAmmo;

struct AIProcess
{
    bool SetCurrentAmmo(TESAmmo* apAmmo) noexcept;

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

struct HighProcessData
{
    uint8_t pad0[0x218];
    char* strVoiceSubtitle;
    GameArray<std::tuple<uint32_t, void*>> KnowledgeArray; // BSTuple, std::tuple is prolly wrong
};

static_assert(offsetof(AIProcess, movementType) == 0x137);
