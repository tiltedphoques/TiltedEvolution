#pragma once

struct ActiveEffect;
struct BGSLoadFormBuffer;

struct InventoryEntry;

struct MiddleProcess
{
    // void SaveActiveEffects()
    void LoadActiveEffects(BGSLoadFormBuffer* apLoadGameBuffer);

    uint8_t pad0[0xB0];
    float pitch;
    uint8_t padB4[0xB8 - 0xB4];
    float direction; // B8
    uint8_t padBC[0x1A0 - 0xBC];
    GameList<ActiveEffect>* ActiveEffects;
    uint8_t pad1A8[0x218 - 0x1A8];
    BSPointerHandle<TESObjectREFR> commandingActor;
    uint8_t pad21C[0x220 - 0x21C];
    InventoryEntry* leftEquippedObject;
    uint8_t pad228[0x260 - 0x228];
    InventoryEntry* rightEquippedObject;
    InventoryEntry* ammoEquippedObject; // could be more than just ammo
    uint8_t pad270[0x326 - 0x270];
    bool lookAt;
};

static_assert(offsetof(MiddleProcess, pitch) == 0xB0);
static_assert(offsetof(MiddleProcess, direction) == 0xB8);
static_assert(offsetof(MiddleProcess, leftEquippedObject) == 0x220);
static_assert(offsetof(MiddleProcess, rightEquippedObject) == 0x260);
static_assert(offsetof(MiddleProcess, lookAt) == 0x326);
