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

struct Actor : TESObjectREFR
{
    static constexpr FormType Type = FormType::Character;

    // Allocs and calls constructor
    static GamePtr<Actor> New() noexcept;
    // Places a brand new actor in the world
    static GamePtr<Actor> Create(TESNPC* apBaseForm) noexcept;

    virtual void sub_9C();
    virtual void sub_9D();
    virtual void sub_9E();
    virtual void sub_9F();
    virtual void sub_A0();
    virtual void UnequipObject(void* apUnk1, TESBoundObject* apObject, int32_t aUnk2, void* aUnk3);
    virtual void sub_A2();
    virtual void sub_A3();
    virtual void sub_A4();
    virtual void sub_A5();
    virtual void SetWeaponDrawn(bool aDraw);
    virtual void sub_A7();
    virtual void sub_A8();
    virtual void SetPosition(const NiPoint3& acPoint, bool aSyncHavok = true);
    virtual void sub_AA();
    virtual void Resurrect(bool aResetInventory);
    virtual void sub_AC();
    virtual void sub_AD();
    virtual void sub_AE();
    virtual void sub_AF();
    virtual void sub_B0();
    virtual void sub_B1();
    virtual void sub_B2();
    virtual void sub_B3();
    virtual void sub_B4();
    virtual void sub_B5();
    virtual void sub_B6();
    virtual void sub_B7();
    virtual void sub_B8();
    virtual void sub_B9();
    virtual void sub_BA();
    virtual void PayFine(TESFaction* apFaction, bool aGoToJail, bool aRemoveStolenItems);
    virtual void sub_BC();
    virtual void sub_BD();
    virtual void sub_BE();
    virtual void sub_BF();
    virtual void sub_C0();
    virtual void sub_C1();
    virtual void sub_C2();
    virtual void SetRefraction(bool aEnable, float aRefraction);
    virtual void sub_C4();
    virtual void sub_C5();
    virtual void sub_C6();
    virtual void sub_C7();
    virtual void sub_C8();
    virtual void sub_C9();
    virtual void sub_CA();
    virtual void sub_CB();
    virtual void sub_CC();
    virtual void AttachArrow();
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
    virtual void PutCreatedPackage(struct TESPackage*, bool = false, bool = true, bool = true); // 14069BBF0
    virtual void UpdateAlpha();
    virtual void sub_E1();
    virtual void sub_E2();
    virtual void sub_E3();
    virtual void sub_E4();
    virtual void sub_E5();
    virtual void sub_E6();
    virtual void sub_E7();
    virtual void sub_E8();
    virtual void sub_E9();
    virtual void sub_EA();
    virtual void sub_EB();
    virtual void sub_EC();
    virtual void sub_ED();
    virtual void sub_EE();
    virtual void sub_EF();
    // these 4 are not virtual funcs in fallout 4
    virtual bool MoveToHigh();
    virtual bool MoveToLow();
    virtual bool MoveToMiddleLow();
    virtual bool MoveToMiddleHigh();
    virtual bool sub_F4();
    virtual void sub_F5();
    virtual void sub_F6();
    virtual void sub_F7();
    virtual void sub_F8();
    virtual void sub_F9();
    virtual void sub_FA();
    virtual void sub_FB();
    virtual void sub_FC();
    virtual void sub_FD();
    virtual void sub_FE();
    virtual void sub_FF();
    virtual void sub_100();
    virtual void sub_101();
    virtual void sub_102();
    virtual void sub_103();
    virtual void sub_104();
    virtual void sub_105();
    virtual void sub_106();
    virtual void sub_107();
    virtual void sub_108();
    virtual void InitializeMovementControllerNPC();
    virtual void sub_10A();
    virtual void sub_10B();
    virtual void sub_10C();
    virtual void sub_10D();
    virtual void KillImpl(Actor* apAttacker, float aDamage, bool aSendEvent, bool aRagdollInstant);
    virtual void sub_10F();
    virtual void sub_110();
    virtual void sub_111();
    virtual void sub_112();
    virtual void sub_113();
    virtual void sub_114();
    virtual void sub_115();
    virtual void sub_116();
    virtual void sub_117();
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

    // Real functions
    void DualCastSpell(TESObjectREFR* apDesiredTarget) noexcept;
    void InterruptCast(bool abRefund) noexcept;

    // Casting
    ActorExtension* GetExtension() noexcept;
    ExActor* AsExActor() noexcept;
    ExPlayerCharacter* AsExPlayerCharacter() noexcept;

    // Getters
    float GetSpeed() noexcept;
    TESForm* GetEquippedWeapon(uint32_t aSlotId) const noexcept;
    TESForm* GetEquippedAmmo() const noexcept;
    // in reality this is a BGSLocation
    TESForm *GetCurrentLocation();
    float GetActorValue(uint32_t aId) const noexcept;
    float GetActorPermanentValue(uint32_t aId) const noexcept;
    Inventory GetActorInventory() const noexcept;
    MagicEquipment GetMagicEquipment() const noexcept;
    Inventory GetEquipment() const noexcept;
    int32_t GetGoldAmount() noexcept;
    uint16_t GetLevel() noexcept;

    Factions GetFactions() const noexcept;
    ActorValues GetEssentialActorValues() const noexcept;

    // Setters
    void SetSpeed(float aSpeed) noexcept;
    void SetLevelMod(uint32_t aLevel) noexcept;
    void SetActorValue(uint32_t aId, float aValue) noexcept;
    void ForceActorValue(ActorValueOwner::ForceMode aMode, uint32_t aId, float aValue) noexcept;
    void SetActorValues(const ActorValues& acActorValues) noexcept;
    void SetFactions(const Factions& acFactions) noexcept;
    void SetFactionRank(const TESFaction* apFaction, int8_t aRank) noexcept;
    void ForcePosition(const NiPoint3& acPosition) noexcept;
    void SetWeaponDrawnEx(bool aDraw) noexcept;
    void SetPackage(TESPackage* apPackage) noexcept;
    void SetActorInventory(const Inventory& aInventory) noexcept;
    void SetMagicEquipment(const MagicEquipment& acEquipment) noexcept;
    void SetEssentialEx(bool aSet) noexcept;
    void SetNoBleedoutRecovery(bool aSet) noexcept;
    void SetPlayerRespawnMode() noexcept;
    void SetPlayerTeammate(bool aSet) noexcept;

    // Actions
    void UnEquipAll() noexcept;
    void RemoveFromAllFactions() noexcept;
    void QueueUpdate() noexcept;
    bool InitiateMountPackage(Actor* apMount) noexcept;
    void GenerateMagicCasters() noexcept;
    void DispelAllSpells(bool aNow = false) noexcept;

    bool IsDead() noexcept;
    bool IsDragon() noexcept;
    void Kill() noexcept;
    void Reset() noexcept;
    void Respawn() noexcept;
    void PickUpObject(TESObjectREFR* apObject, int32_t aCount, bool aUnk1, float aUnk2) noexcept;
    void DropObject(TESBoundObject* apObject, ExtraDataList* apExtraData, int32_t aCount, NiPoint3* apLocation, NiPoint3* apRotation) noexcept;
    void DropOrPickUpObject(const Inventory::Entry& arEntry, NiPoint3* apPoint, NiPoint3* apRotate) noexcept;
    void SpeakSound(const char* pFile);

    enum ActorFlags
    {
        IS_A_MOUNT = 1 << 1,
        IS_ESSENTIAL = 1 << 18,
    };

    bool IsMount() const noexcept
    {
        return flags2 & ActorFlags::IS_A_MOUNT;
    }

    bool IsEssential() const noexcept
    {
        return flags2 & ActorFlags::IS_ESSENTIAL;
    }
    void SetEssential(bool aSetEssential) noexcept
    {
        if (aSetEssential)
            flags2 |= ActorFlags::IS_ESSENTIAL;
        else
            flags2 &= ~ActorFlags::IS_ESSENTIAL;
    }

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

    MagicTarget magicTarget;
    ActorValueOwner actorValueOwner;
    ActorState actorState;
    BSTEventSink<void*> tdEvent;
    BSTEventSink<void*> cmfEvent;
    IPostAnimationChannelUpdateFunctor unk78;
    uint32_t flags1;
    float headTrackingUpdateDelay;
    uint32_t unk84;
    AIProcess* currentProcess;
    uint32_t dialogueHandle;
    uint32_t combatHandle;
    uint32_t killerHandle;
    uint32_t unk98;
    float fVoiceTimer;
    uint32_t unkA0;
    uint32_t unkA4;
    int32_t iThiefCrimeStamp;
    int32_t iActionValue;
    float fTimerOnAction;
    uint32_t unkB4[5];
    void* pad138[2];
    struct IMovementDrivenControl* movementDrivenControl; // MovementControllerNPC
    uint32_t unk0D4;
    CombatController* pCombatController;
    void* unkDC;
    uint32_t unkE0;
    uint32_t unkE4;
    uint32_t unkE8;
    uint32_t unkEC;
    uint32_t unk178; // F0
    uint32_t unk17C; // F4
    SpellItemEntry* spellItemHead; // F8
    BSTSmallArray<void*> addedSpells;
    ActorMagicCaster* casters[4];
    MagicItem* magicItems[4];
    TESForm* equippedShout;
    uint32_t someRefrHandle;
    TESRace* race;
    float weight; // set to -1.0 by extra container changes data ctor, asuming it's the weight 
    uint32_t flags2;
    void* unk200[4];
    struct BGSDialogueBranch* dialogueBranch;
    ActorValueModifiers healthModifiers;
    ActorValueModifiers staminaModifiers;
    ActorValueModifiers magickaModifiers;
    ActorValueModifiers voiceModifiers;
    float fLastUpdate;
    uint32_t iLastSeenTime;
    void* actorWeightData;
    float fArmorRating;
    float fArmorBaseFactorSum;
    uint8_t unk194;
    uint8_t unk195;
    uint8_t unk196;
    uint8_t flag197;

    uint8_t pad198[8];
    BSRecursiveLock actorLock;
    uint8_t padActorEnd[0x2B0 - 0x284];

    //void Save_Reversed(uint32_t aChangeFlags, Buffer::Writer& aWriter);    
};

static_assert(offsetof(Actor, currentProcess) == 0xF0);
static_assert(offsetof(Actor, flags1) == 0xE0);
static_assert(offsetof(Actor, actorValueOwner) == 0xB0);
static_assert(offsetof(Actor, actorState) == 0xB8);
static_assert(offsetof(Actor, flags2) == 0x1FC);
static_assert(offsetof(Actor, unk194) == 0x270);
static_assert(offsetof(Actor, fVoiceTimer) == 0x108);
static_assert(offsetof(Actor, unk84) == 0xE8);
static_assert(offsetof(Actor, unk17C) == 0x17C);
static_assert(offsetof(Actor, pCombatController) == 0x158);
static_assert(offsetof(Actor, magicItems) == 0x1C0);
static_assert(offsetof(Actor, equippedShout) == 0x1E0);
static_assert(offsetof(Actor, actorLock) == 0x27C);
static_assert(sizeof(Actor) == 0x2B0);
static_assert(sizeof(Actor::SpellItemEntry) == 0x18);
