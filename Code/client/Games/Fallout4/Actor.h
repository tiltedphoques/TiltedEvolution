#pragma once

#include <Games/Events.h>
#include <TESObjectREFR.h>

#include <Magic/MagicTarget.h>
#include <Forms/TESActorBase.h>
#include <Misc/ActorState.h>
#include <Misc/IPostAnimationChannelUpdateFunctor.h>
#include <Forms/MagicItem.h>
#include <Magic/ActorMagicCaster.h>

#include <Structs/Inventory.h>
#include <Structs/Factions.h>
#include <Structs/ActorValues.h>

struct TESNPC;
struct TESRace;
struct ExActor;
struct ExPlayerCharacter;
struct ActorExtension;
struct AIProcess;
struct CombatController;
struct TESIdleForm;
struct BGSDialogueBranch;
struct MovementControllerNPC;

// Fallout 4 Actor: 0x490. Base chain after TESObjectREFR (0x110):
// MagicTarget 0x110, ActorState 0x128, six event sinks, a functor and four
// event sources, members start at 0x2D0.
struct BSMovementDataChangedEvent;
struct BSTransformDeltaEvent;
struct BSSubGraphActivationUpdate;
struct bhkCharacterMoveFinishEvent;
struct bhkNonSupportContactEvent;
struct bhkCharacterStateChangeEvent;
struct MovementMessageUpdateRequestImmediate;
struct PerkValueChangedEvent;
struct PerkEntryUpdatedEvent;
struct ActorCPMEvent;

template <class T> struct FO4EventSink
{
    virtual ~FO4EventSink() = default;
    virtual BSTEventResult OnEvent(const T*) { return BSTEventResult::kOk; }
};

template <class T> struct FO4EventSourceT
{
    virtual ~FO4EventSourceT() = default;
    void* listeners;     // BSTArray-ish head
    void* data;
    uint32_t size;
    uint32_t capacity;
    void* lock;
    uint8_t pad[0x58 - 0x28];
};
static_assert(sizeof(FO4EventSourceT<int>) == 0x58);

struct Actor
    : TESObjectREFR,                                      // 000
      MagicTarget,                                        // 110
      ActorState,                                         // 128
      FO4EventSink<BSMovementDataChangedEvent>,           // 138
      FO4EventSink<BSTransformDeltaEvent>,                // 140
      FO4EventSink<BSSubGraphActivationUpdate>,           // 148
      FO4EventSink<bhkCharacterMoveFinishEvent>,          // 150
      FO4EventSink<bhkNonSupportContactEvent>,            // 158
      FO4EventSink<bhkCharacterStateChangeEvent>,         // 160
      IPostAnimationChannelUpdateFunctor,                 // 168
      FO4EventSourceT<MovementMessageUpdateRequestImmediate>,   // 170
      FO4EventSourceT<PerkValueChangedEvent>,                    // 1C8
      FO4EventSourceT<PerkEntryUpdatedEvent>,                    // 220
      FO4EventSourceT<ActorCPMEvent>                             // 278
{
    static constexpr FormType Type = FormType::Character;

    static GamePtr<Actor> New() noexcept;
    static GamePtr<Actor> Create(TESNPC* apBaseForm) noexcept;
    static GamePtr<Actor> Spawn(uint32_t aBaseFormId) noexcept;

    virtual void sub_C6();
    virtual void sub_C7();
    virtual void sub_C8();
    virtual void DrawWeaponMagicHands(bool aDraw);
    virtual void SetPosition(const NiPoint3& acPoint, bool aUpdateCharController);
    virtual void sub_CB();
    virtual void Resurrect(bool aResetInventory, bool aAttach3D);
    virtual void sub_CD();
    virtual void sub_CE();
    virtual void sub_CF();
    virtual void sub_D0();
    virtual void sub_D1();
    virtual void sub_D2();
    virtual void sub_D3();
    virtual void sub_D4();
    virtual void sub_D5();
    virtual void sub_D6();
    virtual void sub_D7();
    virtual void sub_D8();
    virtual void sub_D9();
    virtual void sub_DA();
    virtual void sub_DB();
    virtual void sub_DC();
    virtual void sub_DD();
    virtual void sub_DE();
    virtual void sub_DF();
    virtual void sub_E0();
    virtual void sub_E1();
    virtual void sub_E2();
    virtual void sub_E3();
    virtual void sub_E4();
    virtual void sub_E5();
    virtual void sub_E6();
    virtual void sub_E7();
    virtual void SetRefraction(bool aEnable, float aRefraction);
    virtual void sub_E9();
    virtual void sub_EA();
    virtual void sub_EB();
    virtual void sub_EC();
    virtual void sub_ED();
    virtual void sub_EE();
    virtual void sub_EF();
    virtual void sub_F0();
    virtual void sub_F1();
    virtual void sub_F2();
    virtual void sub_F3();
    virtual void sub_F4();
    virtual void sub_F5();
    virtual void sub_F6();
    virtual void sub_F7();
    virtual void sub_F8();
    virtual void sub_F9();
    virtual void PutCreatedPackage(TESPackage* apPack, bool aTempPack, bool aIsACreatedPackage, bool aAllowFromFurniture);
    virtual void UpdateAlpha();
    virtual void sub_FC();
    virtual void sub_FD();
    virtual void sub_FE();
    virtual void sub_FF();
    virtual void StopCombat();
    virtual void sub_101();
    virtual void sub_102();
    virtual void sub_103();
    virtual void sub_104();
    virtual void sub_105();
    virtual void sub_106();
    virtual void sub_107();
    virtual void sub_108();
    virtual void sub_109();
    virtual void sub_10A();
    virtual void sub_10B();
    virtual void sub_10C();
    virtual void sub_10D();
    virtual void sub_10E();
    virtual void sub_10F();
    virtual void sub_110();
    virtual void sub_111();
    virtual void sub_112();
    virtual void sub_113();
    virtual void sub_114();
    virtual void sub_115();
    virtual void sub_116();
    virtual void KillImpl(Actor* apAttacker, float aDamage, bool aSendEvent, bool aRagdollInstant);
    virtual void sub_118();
    virtual void sub_119();
    virtual void sub_11A();
    virtual void sub_11B();
    virtual void sub_11C();
    virtual void sub_11D();
    virtual void sub_11E();
    virtual void sub_11F();
    virtual void sub_120();
    virtual void sub_121();
    virtual void sub_122();
    virtual void sub_123();
    virtual void sub_124();
    virtual void sub_125();
    virtual void sub_126();
    virtual void sub_127();
    virtual void sub_128();
    virtual void sub_129();
    virtual void sub_12A();
    virtual void sub_12B();
    virtual void sub_12C();
    virtual void sub_12D();
    virtual void sub_12E();
    virtual void sub_12F();
    virtual void sub_130();
    virtual void sub_131();
    virtual void sub_132();

    // Real functions
    void DualCastSpell(TESObjectREFR* apDesiredTarget) noexcept;
    void InterruptCast(bool abRefund) noexcept;

    // Casting
    ActorExtension* GetExtension() noexcept;
    ExActor* AsExActor() noexcept;
    ExPlayerCharacter* AsExPlayerCharacter() noexcept;

    // Generic client code accesses the state through this; in FO4 the
    // ActorState is a base subobject at 0x128, in Skyrim a member.
    ActorState* GetActorState() noexcept { return static_cast<ActorState*>(this); }

    // Getters
    float GetSpeed() noexcept;
    TESForm* GetEquippedWeapon(uint32_t aSlotId) const noexcept;
    TESForm* GetEquippedAmmo() const noexcept;
    Actor* GetCommandingActor() const noexcept;
    TESForm* GetCurrentLocation();
    float GetActorValue(uint32_t aId) const noexcept;
    float GetActorPermanentValue(uint32_t aId) const noexcept;
    Inventory GetActorInventory() const noexcept;
    MagicEquipment GetMagicEquipment() const noexcept;
    Inventory GetEquipment() const noexcept;
    int32_t GetGoldAmount() const noexcept;
    TESNPC* GetLeveledPick() const noexcept;
    uint16_t GetLevel() const noexcept;
    Factions GetFactions() const noexcept;
    ActorValues GetEssentialActorValues() const noexcept;
    [[nodiscard]] bool IsDead() const noexcept;
    [[nodiscard]] bool IsDragon() const noexcept;
    [[nodiscard]] bool IsPlayerSummon() const noexcept;
    [[nodiscard]] bool IsInCombat() const noexcept;
    [[nodiscard]] Actor* GetCombatTarget() const noexcept;
    [[nodiscard]] bool HasPerk(uint32_t aPerkFormId) const noexcept;
    [[nodiscard]] uint8_t GetPerkRank(uint32_t aPerkFormId) const noexcept;
    [[nodiscard]] bool IsVampireLord() const noexcept;

    // Setters
    void SetSpeed(float aSpeed) noexcept;
    void SetLevelMod(uint32_t aLevel) noexcept;
    void SetActorValue(uint32_t aId, float aValue) noexcept;
    void ForceActorValue(ActorValueOwner::ForceMode aMode, uint32_t aId, float aValue) noexcept;
    void SetActorValues(const ActorValues& acActorValues) noexcept;
    void SetCommandingActor(BSPointerHandle<TESObjectREFR> aCommandingActor) noexcept;
    void SetFactions(const Factions& acFactions) noexcept;
    void SetFactionRank(const TESFaction* apFaction, int8_t aRank) noexcept;
    void ForcePosition(const NiPoint3& acPosition) noexcept;
    void SetWeaponDrawnEx(bool aDraw) noexcept;
    void SetPackage(TESPackage* apPackage) noexcept;
    void SetActorInventory(const Inventory& aInventory) noexcept;
    void SetMagicEquipment(const MagicEquipment& acEquipment) noexcept;
    void SetEssentialEx(bool aSet) noexcept;
    void SetNoBleedoutRecovery(bool aSet) noexcept;
    void SetPlayerRespawnMode(bool aSet = true) noexcept;
    void SetPlayerTeammate(bool aSet) noexcept;

    // Actions
    void UnEquipAll() noexcept;
    void RemoveFromAllFactions() noexcept;
    void QueueUpdate() noexcept;
    bool InitiateMountPackage(Actor* apMount) noexcept;
    void GenerateMagicCasters() noexcept;
    void DispelAllSpells(bool aNow = false) noexcept;
    void Reset() noexcept;
    void Kill() noexcept;
    void Respawn() noexcept;
    void PickUpObject(TESObjectREFR* apObject, int32_t aCount, bool aUnk1, float aUnk2) noexcept;
    void DropObject(TESBoundObject* apObject, ExtraDataList* apExtraData, int32_t aCount, NiPoint3* apLocation, NiPoint3* apRotation) noexcept;
    void DropOrPickUpObject(const Inventory::Entry& arEntry, NiPoint3* apPoint, NiPoint3* apRotate) noexcept;
    void SpeakSound(const char* pFile);
    void StartCombatEx(Actor* apTarget) noexcept;
    void SetCombatTargetEx(Actor* apTarget) noexcept;
    void StartCombat(Actor* apTarget) noexcept;
    bool PlayIdle(TESIdleForm* apIdle) noexcept;
    bool RemoveSpell(MagicItem* apSpell) noexcept;

    enum ActorFlags
    {
        IS_A_MOUNT = 1 << 1,
        IS_COMMANDED_ACTOR = 1 << 16,
        IS_ESSENTIAL = 1 << 18,
    };

    // FO4 BOOL_FLAGS/MORE_FLAGS: essential and commanded live in boolFlags.
    bool IsMount() const noexcept { return boolFlags & ActorFlags::IS_A_MOUNT; }

    bool IsEssential() const noexcept { return boolFlags & ActorFlags::IS_ESSENTIAL; }
    void SetEssential(bool aSetEssential) noexcept
    {
        if (aSetEssential)
            boolFlags |= ActorFlags::IS_ESSENTIAL;
        else
            boolFlags &= ~ActorFlags::IS_ESSENTIAL;
    }

    bool IsCommandedActor() const noexcept { return boolFlags & ActorFlags::IS_COMMANDED_ACTOR; }

public:
    enum ChangeFlags : uint32_t
    {
        CHANGE_ACTOR_LIFESTATE = 1 << 10,
        CHANGE_ACTOR_EXTRA_PACKAGE_DATA = 1 << 11,
        CHANGE_ACTOR_EXTRA_MERCHANT_CONTAINER = 1 << 12,
        CHANGE_ACTOR_EXTRA_DISMEMBERED_LIMBS = 1 << 17,
        CHANGE_ACTOR_LEVELED_ACTOR = 1 << 18,
        CHANGE_ACTOR_DISPOSITION_MODIFIERS = 1 << 19,
        CHANGE_ACTOR_TEMP_MODIFIERS = 1 << 20,
        CHANGE_ACTOR_DAMAGE_MODIFIERS = 1 << 21,
        CHANGE_ACTOR_OVERRIDE_MODIFIERS = 1 << 22,
        CHANGE_ACTOR_PERMANENT_MODIFIERS = 1 << 23,
    };

    struct ActorValueModifiers
    {
        float permanentModifier;
        float temporaryModifier;
        float damageModifier;
    };

    struct SpellItemEntry
    {
        SpellItem* pItem;
        float unk8;
        SpellItemEntry* pNext;
    };

    // members (offsets from libxse/commonlibf4)
    uint32_t niFlags;                 // 0x2D0
    float updateTargetTimer;          // 0x2D4
    NiPoint3 editorLocCoord;          // 0x2D8
    NiPoint3 editorLocRot;            // 0x2E4
    TESForm* editorLocForm;           // 0x2F0
    void* editorLocation;             // 0x2F8 (BGSLocation*)
    AIProcess* currentProcess;        // 0x300
    void* actorMover;                 // 0x308
    void* speakingAnimArchType;       // 0x310
    BSTSmartPointer<MovementControllerNPC> movementController; // 0x318
    TESPackage* initialPackage;       // 0x320
    CombatController* combatController; // 0x328
    TESFaction* vendorFaction;        // 0x330
    uint8_t avStorage[0x370 - 0x338]; // 0x338 ActorValueStorage
    BGSDialogueBranch* exclusiveBranch; // 0x370
    int32_t criticalStage;            // 0x378
    uint32_t dialogueItemTarget;      // 0x37C ObjectRefHandle
    uint32_t currentCombatTarget;     // 0x380
    uint32_t myKiller;                // 0x384
    float checkMyDeadBodyTimer;       // 0x388
    float voiceTimer;                 // 0x38C
    float voiceLengthTotal;           // 0x390
    float underWaterTimer;            // 0x394
    int32_t thiefCrimeStamp;          // 0x398
    int32_t actionValue;              // 0x39C
    float timeronAction;              // 0x3A0
    uint32_t calculateVendorFactionTimer; // 0x3A4
    uint32_t intimidateBribeDayStamp; // 0x3A8
    float equippedWeight;             // 0x3AC
    BSTSmallArray<SpellItem*> addedSpells; // 0x3B0
    ActorMagicCaster* magicCasters[4]; // 0x3C8
    MagicItem* selectedSpell[4];      // 0x3E8
    void* castPowerItems;             // 0x408
    TESForm* selectedPower;           // 0x410
    TESRace* race;                    // 0x418
    void* perks;                      // 0x420
    void* biped;                      // 0x428 BSTSmartPointer<BipedAnim>
    uint8_t addingToOrRemovingFromScene[0x434 - 0x430]; // BSNonReentrantSpinLock
    BSRecursiveLock perkArrayLock;    // 0x434
    uint32_t boolFlags;               // 0x43C
    uint32_t moreFlags;               // 0x440
    ActorValueModifiers healthModifiers;       // 0x444
    ActorValueModifiers actionPointsModifiers; // 0x450
    ActorValueModifiers staminaModifiers;      // 0x45C
    ActorValueModifiers radsModifiers;         // 0x468
    float lastUpdate;                 // 0x474
    uint32_t lastSeenTime;            // 0x478
    float armorRating;                // 0x47C
    float armorBaseFactorSum;         // 0x480
    uint8_t visFlags;                 // 0x484 (4-bit bitfield in engine)
    uint8_t raceSwitchPending;        // 0x488 (1-bit bitfield)
    uint8_t soundCallBackSet;         // 0x489
    bool trespassing;                 // 0x48A
    uint8_t padActorEnd[0x490 - 0x48B];

    // void Save_Reversed(uint32_t aChangeFlags, Buffer::Writer& aWriter);
};

static_assert(offsetof(Actor, currentProcess) == 0x300);
static_assert(offsetof(Actor, combatController) == 0x328);
static_assert(offsetof(Actor, boolFlags) == 0x43C);
static_assert(offsetof(Actor, healthModifiers) == 0x444);
static_assert(sizeof(Actor) == 0x490);
static_assert(sizeof(Actor::SpellItemEntry) == 0x18);
