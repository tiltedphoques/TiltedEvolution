#pragma once

#include <Forms/TESActorBase.h>

#include <Components/TESRaceForm.h>
#include <Components/BGSOverridePackCollection.h>
#include <Components/BGSNativeTerminalForm.h>
#include <Components/BGSAttachParentArray.h>
#include <Components/BGSCharacterTint.h>
#include <Components/BGSForcedLocRefType.h>

struct BGSTextureSet;
struct BGSColorForm;
struct TESClass;
struct TESCombatStyle;
struct BGSOutfit;
struct TESFaction;
struct BGSHeadPart;

struct TESNPC : TESActorBase
{
    static constexpr FormType Type = FormType::Npc;

    static TESNPC* Create(const String& acBuffer, uint32_t aChangeFlags) noexcept;

    // TODO: ft, verify whether it also works like this in fallout 4
    TESNPC* GetTemplateBase() const noexcept
    {
        TESNPC* pTemplate = npcTemplate;

        while (pTemplate && pTemplate->IsTemporary())
            pTemplate = pTemplate->npcTemplate;

        return pTemplate;
    }

    struct Head
    {
        BGSColorForm* hairColor;
        void* unk8;
        BGSTextureSet* textures;
    };

    TESRaceForm raceForm;
    BGSOverridePackCollection overridePackCollection; // 1C0
    TESNPC* npcTemplate;
    BGSForcedLocRefType forcedLocRefType;
    BGSNativeTerminalForm nativeTerminalForm;
    void* menuOpenCloseEventSink;
    BGSAttachParentArray attachParentArray;

    uint32_t unk238; // 238 - set to 0
    uint16_t unk23C; // 23C - set to 0
    uint8_t unk23E; // 23E - set to 1
    uint8_t pad23F; // 23F
    TESClass* npcClass; // 240
    Head* head; // 248 - set to nullptr
    void* unk250; // 250
    TESCombatStyle* combatStyle; // 258
    uint8_t unk260[8]; // 260
    void* unk268; // 268
    void* unk270; // 270
    float smallWeight; // 278
    float muscularWeight; // 27C
    float largeWeight; // 280
    float unk284; // 284
    float unk288; // 288
    uint32_t pad28C; // 28C
    void* unk290; // 290
    void* unk298; // 298
    void* unk2A0; // 2A0 - set to nullptr
    void* unk2A8; // 2A8
    BGSOutfit* outfits[2]; // 2B0
    void* unk2C0; // 2C0
    TESFaction* faction; // 2C8
    BGSHeadPart** headParts; // 2D0
    void* unk2D8; // 2D8
    void* morphRegionData; // 2E0
    uint8_t headPartCount; // 2E8
    uint8_t unk2E9; // 2E9
    uint8_t skinColor[4]; // 2EA
    uint16_t pad2EE; // 2EE
    uint64_t unk2F0; // 2F0 - not sure about type
    void* unk2F8; // 2F8
    GameArray<BGSCharacterTint::Entry*>* tints; // 300

    BGSHeadPart* GetHeadPart(uint32_t aType);
    void Serialize(String* apSaveBuffer) const noexcept;
    void Deserialize(const String& acBuffer, uint32_t aChangeFlags) noexcept;
    void Initialize() noexcept;
};

static_assert(offsetof(TESNPC, unk238) == 0x238);
static_assert(offsetof(TESNPC, pad28C) == 0x28C);
static_assert(offsetof(TESNPC, headPartCount) == 0x2E8);
static_assert(offsetof(TESNPC, unk2F8) == 0x2F8);
static_assert(offsetof(TESNPC, tints) == 0x300);
static_assert(sizeof(TESNPC) == 0x308);
