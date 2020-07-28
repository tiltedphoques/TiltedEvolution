#pragma once

#if TP_FALLOUT4

#include <Games/Fallout4/TESObjectREFR.h>
#include <Games/Fallout4/Misc/MagicTarget.h>
#include <Games/Fallout4/Misc/ActorState.h>
#include <Games/Fallout4/Forms/TESForm.h>
#include <Structs/Inventory.h>

#include <Games/Events.h>
#include <Games/Fallout4/Events.h>

struct ExActor;
struct ExPlayerCharacter;
struct ActorExtension;
struct TESNPC;
struct ProcessManager;

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

    // Casting
    ActorExtension* GetExtension() noexcept;
    ExActor* AsExActor() noexcept;
    ExPlayerCharacter* AsExPlayerCharacter() noexcept;

    // Getters
    float GetSpeed() noexcept;
    TESForm* GetEquippedWeapon(uint32_t aSlotId) const noexcept;
    Inventory GetInventory() const noexcept;

    // Setters
    void SetSpeed(float aSpeed) noexcept;
    void SetLevelMod(uint32_t aLevel) noexcept;
    void SetInventory(const Inventory& acInventory) noexcept;
    void ForcePosition(const NiPoint3& acPosition) noexcept;

    // Actions
    void UnEquipAll() noexcept;
    void QueueUpdate() noexcept;

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

#endif
