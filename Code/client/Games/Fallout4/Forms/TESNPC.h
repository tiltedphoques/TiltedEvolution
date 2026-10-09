#pragma once

#include <Forms/TESActorBase.h>

#include <Components/TESRaceForm.h>
#include <Components/BGSOverridePackCollection.h>

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
    static constexpr FormType Type = FormType::Npc;

    static TESNPC* Create(const String& acBuffer, uint32_t aChangeFlags) noexcept;

    TESNPC* GetTemplateBase() const noexcept
    {
        TESNPC* pTemplate = faceNPC;

        while (pTemplate && pTemplate->IsTemporary())
            pTemplate = pTemplate->faceNPC;

        return pTemplate;
    }

    struct HeadData
    {
        BGSColorForm* hairColor;
        BGSColorForm* facialHairColor;
        BGSTextureSet* headTexture;
    };

    TESRaceForm raceForm;
    BGSOverridePackCollection overridePacks;
    uint8_t forcedLocRefType[0x10];
    uint8_t nativeTerminalForm[0x10];
    void* menuEventSink;
    uint8_t attachParents[0x18];
    uint8_t npcData[8];
    TESClass* npcClass;
    HeadData* headData;
    TESForm* giftFilter;
    TESCombatStyle* combatStyle;
    uint32_t fileOffset;
    uint32_t pad264;
    TESRace* originalRace;
    TESNPC* faceNPC;
    NiPoint3 morphWeight;
    float height;
    float heightMax;
    uint32_t pad28C;
    void* sounds;
    BSFixedString shortName;
    TESObjectARMO* farSkin;
    TESForm* powerArmorFurniture;
    BGSOutfit* defaultOutfit;
    BGSOutfit* sleepOutfit;
    TESForm* defaultPackList;
    TESFaction* faction;
    BGSHeadPart** headparts;
    GameArray<float>* morphRegionSliderValues;
    void* facialBoneRegionSliderValues;
    uint8_t headpartsCount;
    uint8_t soundLevel;

    struct Color
    {
        uint8_t red, green, blue, alpha;
    } color;
    uint16_t pad2EE;

    GameArray<BGSRelationship*>* relationships;
    void* morphSliderValues;
    void* tintingData;

    BGSHeadPart* GetHeadPart(uint32_t aType);
    void Serialize(String* apSaveBuffer) const noexcept;
    void Deserialize(const String& acBuffer, uint32_t aChangeFlags) noexcept;
    void Initialize() noexcept;
};

static_assert(offsetof(TESNPC, npcClass) == 0x240);
static_assert(offsetof(TESNPC, faceNPC) == 0x270);
static_assert(offsetof(TESNPC, morphWeight) == 0x278);
static_assert(offsetof(TESNPC, headparts) == 0x2D0);
static_assert(offsetof(TESNPC, color) == 0x2EA);
static_assert(offsetof(TESNPC, relationships) == 0x2F0);
static_assert(sizeof(TESNPC) == 0x308);
