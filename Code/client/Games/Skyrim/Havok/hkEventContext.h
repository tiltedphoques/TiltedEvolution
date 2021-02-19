#pragma once
struct hkbCharacter;
struct ahkpWorld;
struct CharacterContext;

struct hkEventContext
{
    hkEventContext(hkbCharacter* apCharacter, ahkpWorld* apWorld);

    hkbCharacter* character;
    struct hkbBehaviorGraph* behaviorGraph;
    void* unk10;
    CharacterContext* characterContext;
    void* unk20;
    void* unk28;
    uint8_t byte30;
    uint8_t pad31[0x38 - 0x31];
    ahkpWorld* hkpWorld;
    void* unk40;
    void* unk48;
};

static_assert(offsetof(hkEventContext, character) == 0);
static_assert(offsetof(hkEventContext, hkpWorld) == 0x38);
static_assert(offsetof(hkEventContext, byte30) == 0x30);
static_assert(offsetof(hkEventContext, unk48) == 0x48);
static_assert(sizeof(hkEventContext) == 0x50);
