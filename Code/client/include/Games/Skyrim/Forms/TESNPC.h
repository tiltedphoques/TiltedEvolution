#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Forms/TESActorBase.h>

#include <Games/Skyrim/Components/TESRaceForm.h>
#include <Games/Skyrim/Components/BGSOverridePackCollection.h>

struct BGSColorForm;
struct BGSTextureSet;
struct TESClass;
struct TESCombatStyle;
struct TESObjectARMO;
struct BGSOutfit;
struct BGSHeadPart;
struct BGSRelationship;

struct TESNPC : TESActorBase
{
    static constexpr uint32_t Type = FormType::Npc;

    static TESNPC* Create(const std::string& acBuffer, uint32_t aChangeFlags) noexcept;

    struct FaceMorphs
    {
        float option[19];
        uint32_t presets[4];

        void CopyFrom(const FaceMorphs& acRhs)
        {
            std::copy(std::begin(acRhs.option), std::end(acRhs.option), std::begin(option));
            std::copy(std::begin(acRhs.presets), std::end(acRhs.presets), std::begin(presets));
        }
    };

    struct HeadData
    {
        BGSColorForm* hairColor;
        BGSTextureSet* headTexture;
    };

    TESRaceForm    raceForm;
    BGSOverridePackCollection overridePacks;
    void* unkDC;
    uint8_t unk0E0[0x24];
    uint8_t pad1B4[0x6];
    uint16_t unk10A;
    TESClass* npcClass;

    HeadData* headData;
    uintptr_t unk114;
    TESCombatStyle* combatStyle;
    size_t unk11C;
    TESRace* overlayRace;
    TESNPC* npcTemplate;
    float height;
    float weight;
    uintptr_t unk130;
    BSFixedString shortName;
    TESObjectARMO* farArmo;
    BGSOutfit* outfits[2];
    uintptr_t unk144;
    TESFaction* faction;

    BGSHeadPart** headparts;
    uint8_t headpartsCount;

#if TP_PLATFORM_64
    uint8_t pad241[5];
#else
    uint8_t pad151[3];
#endif

    struct Color
    {
        uint8_t   red, green, blue;
    } color;

    GameArray<BGSRelationship*>* relationships;
    FaceMorphs* faceMorphs;
    uintptr_t unk160;

    BGSHeadPart* GetHeadPart(uint32_t aType);
    void Serialize(std::string* apSaveBuffer) const noexcept;
    void Deserialize(const std::string& acBuffer, uint32_t aChangeFlags) noexcept;
    void Initialize() noexcept;
};

#ifdef _WIN64
static_assert(offsetof(TESNPC, npcClass) == 0x1C0);
static_assert(offsetof(TESNPC, color) == 0x246);
static_assert(offsetof(TESNPC, relationships) == 0x250);
#else
static_assert(offsetof(TESNPC, color) == 0x154);
static_assert(offsetof(TESNPC, npcClass) == 0x10C);
static_assert(offsetof(TESNPC, relationships) == 0x158);
#endif

#endif
