#pragma once

#include <Forms/TESForm.h>
#include <Misc/ActorState.h>
#include <Misc/MagicTarget.h>
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
struct ProcessManager;
struct TESFaction;

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

    // Setters
    void SetSpeed(float aSpeed) noexcept;
    void SetLevelMod(uint32_t aLevel) noexcept;
    void SetInventory(const Inventory& acInventory) noexcept;
    void SetActorValues(const ActorValues& acActorValues) noexcept;
    void SetFactions(const Factions& acFactions) noexcept;
    void SetFactionRank(const TESFaction* acpFaction, int8_t aRank) noexcept;
    void ForcePosition(const NiPoint3& acPosition) noexcept;

    // Actions
    void UnEquipAll() noexcept;
    void QueueUpdate() noexcept;
    void RemoveFromAllFactions() noexcept;

    bool IsDead() noexcept;
    void Kill() noexcept;
    void Reset() noexcept;
    void Respawn() noexcept;

    MagicTarget magicTarget;
    uint8_t unk118[0x128 - 0x118];
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
    ProcessManager* processManager;

    uint8_t pad308[0x3E8 - 0x308];
    TESForm* magicItems[4];
    uint8_t padActorEnd[0x490 - 0x408];
};

static_assert(sizeof(Actor) == 0x490);
static_assert(offsetof(Actor, magicItems) == 0x3E8);
