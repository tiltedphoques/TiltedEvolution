#pragma once

#include <Forms/TESForm.h>
#include <Misc/ActorState.h>
#include <Magic/MagicTarget.h>
#include <Structs/ActorValues.h>
#include <Structs/Factions.h>
#include <Structs/Inventory.h>
#include <TESObjectREFR.h>

#include <Events.h>
#include <Games/Events.h>

struct ExActor;
struct ExPlayerCharacter;
struct ActorExtension;
struct TESNPC;
struct AIProcess;
struct TESFaction;
struct TESPackage;

struct Actor : TESObjectREFR
{
    static constexpr uint32_t Type = FormType::Character;

    static GamePtr<Actor> New() noexcept;
    static GamePtr<Actor> Create(TESNPC* apNpc) noexcept;

    virtual void sub_C4();
    virtual void sub_C5();
    virtual void sub_C6();
    virtual void sub_C7();
    virtual void sub_C8();
    virtual void SetWeaponDrawn(bool aIsDrawn);
    virtual void SetPosition(const NiPoint3& acPosition, bool aSyncHavok = true);
    virtual void sub_CB();
    virtual void Resurrect(bool aResetInventory);
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
    virtual void sub_E8();
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
    virtual void sub_FA();
    virtual void sub_FB();
    virtual void sub_FC();
    virtual void sub_FD();
    virtual void sub_FE();
    virtual void PutCreatedPackage(struct TESPackage*, bool = false, bool = true, bool = true);
    virtual void sub_100();
    //...
    //virtual void sub_132();

    // Casting
    ActorExtension* GetExtension() noexcept;
    ExActor* AsExActor() noexcept;
    ExPlayerCharacter* AsExPlayerCharacter() noexcept;

    // Getters
    float GetSpeed() noexcept;
    TESForm* GetEquippedWeapon(uint32_t aSlotId) const noexcept;
    Inventory GetInventory() const noexcept;
    Factions GetFactions() const noexcept;
    ActorValues GetEssentialActorValues() noexcept;
    float GetActorValue(uint32_t aId) const noexcept;
    float GetActorPermanentValue(uint32_t aId) const noexcept;
    void* GetCurrentWeapon(void* apResult, uint32_t aEquipIndex) noexcept;

    // Setters
    void SetSpeed(float aSpeed) noexcept;
    void SetLevelMod(uint32_t aLevel) noexcept;
    void SetInventory(const Inventory& acInventory) noexcept;
    void SetActorValue(uint32_t aId, float aValue) noexcept;
    void ForceActorValue(uint32_t aMode, uint32_t aId, float aValue) noexcept;
    void SetActorValues(const ActorValues& acActorValues) noexcept;
    void SetFactions(const Factions& acFactions) noexcept;
    void SetFactionRank(const TESFaction* acpFaction, int8_t aRank) noexcept;
    void ForcePosition(const NiPoint3& acPosition) noexcept;
    void SetWeaponDrawnEx(bool aDraw) noexcept;
    void SetPackage(TESPackage* apPackage) noexcept;

    // Actions
    void UnEquipAll() noexcept;
    void QueueUpdate() noexcept;
    void RemoveFromAllFactions() noexcept;

    bool IsDead() noexcept;
    void Kill() noexcept;
    void Reset() noexcept;
    void Respawn() noexcept;

    MagicTarget magicTarget;
    ActorState actorState;
    BSTEventSink<BSMovementDataChangedEvent> movementDataChangedSink;
    BSTEventSink<BSTransformDeltaEvent> transformDeltaSink;
    BSTEventSink<BSSubGraphActivationUpdate> subGraphActivationUpdateSink;
    BSTEventSink<bhkCharacterMoveFinishEvent> characterMoveFinishSink;
    BSTEventSink<bhkNonSupportContactEvent> nonSupportContactSink;
    BSTEventSink<bhkCharacterStateChangeEvent> characterStateChangeSink;
    void* postAnimationChannelUpdateFunctor;
    uint8_t unk170[0x2D0 - 0x170];
    uint32_t actorFlags;
    uint8_t unk2D8[0x300 - 0x2D8];
    AIProcess* currentProcess;

    uint8_t pad308[0x3E8 - 0x308];
    TESForm* magicItems[4];
    uint8_t padActorEnd[0x490 - 0x408];
};

static_assert(sizeof(Actor) == 0x490);
static_assert(offsetof(Actor, magicItems) == 0x3E8);
