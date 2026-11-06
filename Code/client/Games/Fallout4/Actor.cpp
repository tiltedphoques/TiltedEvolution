#include <TiltedOnlinePCH.h>

#include <Games/References.h>
#include <Games/Memory.h>
#include <EquipManager.h>
#include <Forms/ActorValueInfo.h>
#include <Components/BGSKeywordForm.h>
#include <Forms/TESNPC.h>
#include <Games/TES.h>
#include <Games/Overrides.h>
#include <Forms/TESFaction.h>
#include <AI/AIProcess.h>
#include <Havok/bhkCharacterController.h>
#include <Misc/MiddleProcess.h>
#include <ExtraData/ExtraFactionChanges.h>
#include <ExtraData/ExtraLeveledCreature.h>
#include <Magic/ActorMagicCaster.h>
#include <World.h>
#include <Events/HitEvent.h>
#include <Events/HealthChangeEvent.h>
#include <Events/InventoryChangeEvent.h>
#include <Events/DialogueEvent.h>
#include <Games/Misc/MenuTopicManager.h>
#include <Games/Misc/ActorKnowledge.h>
#include <Forms/TESQuest.h>

ActorExtension* Actor::GetExtension() noexcept
{
    if (auto* pActor = AsExActor())
        return static_cast<ActorExtension*>(pActor);
    if (auto* pPlayer = AsExPlayerCharacter())
        return static_cast<ActorExtension*>(pPlayer);
    return nullptr;
}

ExActor* Actor::AsExActor() noexcept
{
    return formType == Type && this != PlayerCharacter::Get() ? static_cast<ExActor*>(this) : nullptr;
}

ExPlayerCharacter* Actor::AsExPlayerCharacter() noexcept
{
    return this == PlayerCharacter::Get() ? static_cast<ExPlayerCharacter*>(this) : nullptr;
}

GamePtr<Actor> Actor::Create(TESNPC* apBaseForm) noexcept
{
    auto* pPlayer = PlayerCharacter::Get();
    auto* pManager = ModManager::Get();
    if (!apBaseForm || !pPlayer || !pManager)
        return {};

    // Let the engine allocate the actor: references handed to CreateReference skip
    // parts of the setup, which leaves them without animation subgraphs (bind pose).
    auto position = pPlayer->position;
    auto rotation = pPlayer->rotation;
    const uint32_t handle = pManager->Spawn(position, rotation, pPlayer->parentCell, pPlayer->GetWorldSpace(), apBaseForm);

    GamePtr<Actor> pActor = Cast<Actor>(TESObjectREFR::GetByHandle(handle));
    if (!pActor)
        return {};

    pActor->SetSkipSaveFlag(true);
    pActor->GetExtension()->SetRemote(true);
    return pActor;
}

GamePtr<Actor> Actor::Spawn(uint32_t aBaseFormId) noexcept
{
    return Create(Cast<TESNPC>(TESForm::GetById(aBaseFormId)));
}

uint16_t Actor::GetLevel() const noexcept
{
    TP_THIS_FUNCTION(TGetLevel, uint16_t, const Actor);
    static VersionDbPtr<TGetLevel> getLevel(2229734);
    return TiltedPhoques::ThisCall(getLevel, this);
}

float Actor::GetActorValue(uint32_t aId) const noexcept
{
    return actorValueOwner.GetValue(aId);
}

float Actor::GetActorPermanentValue(uint32_t aId) const noexcept
{
    return actorValueOwner.GetPermanentValue(aId);
}

void Actor::SetActorValue(uint32_t aId, float aValue) noexcept
{
    actorValueOwner.SetValue(aId, aValue);
}

void Actor::ForceActorValue(ActorValueOwner::ForceMode aMode, uint32_t aId, float aValue) noexcept
{
    const float initialValue = aMode == ActorValueOwner::ForceMode::PERMANENT ? GetActorPermanentValue(aId) : GetActorValue(aId);
    if (aValue != initialValue)
        actorValueOwner.ForceCurrent(aMode, aId, aValue - initialValue);
}

ActorValues Actor::GetEssentialActorValues() const noexcept
{
    ActorValues values;
    const uint32_t essentialValues[] = {ActorValueInfo::kHealth, ActorValueInfo::kActionPoints, ActorValueInfo::kRads, ActorValueInfo::kRadHealthMax, ActorValueInfo::kFatigue, ActorValueInfo::kFatigueAPMax};
    for (const auto i : essentialValues)
    {
        if (!ActorValueInfo::Resolve(i))
            continue;
        values.ActorValuesList.emplace(i, GetActorValue(i));
        values.ActorMaxValuesList.emplace(i, GetActorPermanentValue(i));
    }
    return values;
}

void Actor::SetActorValues(const ActorValues& acActorValues) noexcept
{
    for (const auto& value : acActorValues.ActorMaxValuesList)
        ForceActorValue(ActorValueOwner::ForceMode::PERMANENT, value.first, value.second);
    for (const auto& value : acActorValues.ActorValuesList)
        ForceActorValue(ActorValueOwner::ForceMode::DAMAGE, value.first, value.second);
}

namespace
{
TP_THIS_FUNCTION(TDestructor, void, Actor);
TDestructor* s_destructor = nullptr;

void TP_MAKE_THISCALL(HookDestructor, Actor)
{
    if (auto* pExtension = apThis->GetExtension())
        pExtension->~ActorExtension();
    TiltedPhoques::ThisCall(s_destructor, apThis);
}

TP_THIS_FUNCTION(TModActorValue, void, Actor, ActorValueOwner::ForceMode, const ActorValueInfo*, float, TESObjectREFR*);
TModActorValue* s_modActorValue = nullptr;

// All health damage ends up here. Damage is applied by the client that owns the
// victim or dealt the hit, and sent to the others, like Skyrim's DamageActor hook.
void TP_MAKE_THISCALL(HookModActorValue, Actor, ActorValueOwner::ForceMode aMode, const ActorValueInfo* apInfo, float aDelta, TESObjectREFR* apSource)
{
    if (aMode != ActorValueOwner::ForceMode::DAMAGE || aDelta == 0.f || ScopedActorValueOverride::IsOverriden() || !apInfo ||
        apInfo != ActorValueInfo::Resolve(ActorValueInfo::kHealth))
    {
        return TiltedPhoques::ThisCall(s_modActorValue, apThis, aMode, apInfo, aDelta, apSource);
    }

    // Healing, regen included (Actor::RestoreActorValue lands here). The owner heals and sends the
    // change, like Skyrim's RegenAttributes and ApplyActorEffect hooks.
    if (aDelta > 0.f)
    {
        if (apThis->GetExtension()->IsRemote())
            return;

        World::Get().GetRunner().Trigger(HealthChangeEvent(apThis->formID, aDelta));
        return TiltedPhoques::ThisCall(s_modActorValue, apThis, aMode, apInfo, aDelta, apSource);
    }

    Actor* pHitter = Cast<Actor>(apSource);
    if (pHitter)
        World::Get().GetRunner().Trigger(HitEvent(pHitter->formID, apThis->formID));

    const auto* pExHittee = apThis->GetExtension();
    if (pExHittee->IsLocalPlayer())
    {
        if (!World::Get().GetServerSettings().PvpEnabled && pHitter && pHitter->GetExtension()->IsRemotePlayer())
            return;

        World::Get().GetRunner().Trigger(HealthChangeEvent(apThis->formID, aDelta));
        return TiltedPhoques::ThisCall(s_modActorValue, apThis, aMode, apInfo, aDelta, apSource);
    }
    if (pExHittee->IsRemotePlayer())
        return;

    if (pHitter)
    {
        const auto* pExHitter = pHitter->GetExtension();
        if (pExHitter->IsLocalPlayer())
        {
            World::Get().GetRunner().Trigger(HealthChangeEvent(apThis->formID, aDelta));
            return TiltedPhoques::ThisCall(s_modActorValue, apThis, aMode, apInfo, aDelta, apSource);
        }
        if (pExHitter->IsRemotePlayer())
            return;
    }

    if (pExHittee->IsLocal())
    {
        World::Get().GetRunner().Trigger(HealthChangeEvent(apThis->formID, aDelta));
        return TiltedPhoques::ThisCall(s_modActorValue, apThis, aMode, apInfo, aDelta, apSource);
    }
}

void QueueActorInventoryChange(Actor* apActor, InventoryChangeEvent aEvent, TESObjectREFR* apTransferReference)
{
    auto ownershipToken = Utils::GetLocalOwnershipToken(apActor->formID);
    if (!ownershipToken && apTransferReference == PlayerCharacter::Get())
        ownershipToken = Utils::GetRemoteOwnershipToken(apActor->formID);

    if (!ownershipToken)
        return;

    aEvent.ServerId = ownershipToken->ServerId;
    aEvent.OwnershipEpoch = ownershipToken->OwnershipEpoch;
    World::Get().GetRunner().Trigger(std::move(aEvent));
}

Inventory::Entry MakeInventoryEntry(const TESBoundObject* apObject, int32_t aCount)
{
    Inventory::Entry item{};
    World::Get().GetModSystem().GetServerModId(apObject->formID, item.BaseId);
    item.Count = aCount;
    return item;
}

TP_THIS_FUNCTION(TAddObjectToContainer, void, Actor, TESBoundObject*, ExtraDataList**, int32_t, TESObjectREFR*, ITEM_REMOVE_REASON);
TAddObjectToContainer* s_addObjectToContainer = nullptr;

// Picking up, buying, crafting and transfers all end up here.
void TP_MAKE_THISCALL(HookAddObjectToContainer, Actor, TESBoundObject* apObject, ExtraDataList** apExtra, int32_t aCount, TESObjectREFR* apOldContainer,
                      ITEM_REMOVE_REASON aReason)
{
    // The local player is sent from HookAddInventoryItem, which this also ends up in.
    if (apObject && aCount > 0 && apThis != PlayerCharacter::Get() && !ScopedInventoryOverride::IsOverriden())
        QueueActorInventoryChange(apThis, InventoryChangeEvent(apThis->formID, MakeInventoryEntry(apObject, aCount)), apOldContainer);

    TiltedPhoques::ThisCall(s_addObjectToContainer, apThis, apObject, apExtra, aCount, apOldContainer, aReason);
}

TP_THIS_FUNCTION(TAddInventoryItem, void, TESObjectREFR, TESBoundObject*, void* apExtra, uint32_t, TESObjectREFR*, const void* apFilter, TESObjectREFR*);
TAddInventoryItem* s_addInventoryItem = nullptr;

// TESObjectREFR::AddInventoryItem. Console and script AddItem, quest rewards and crafting add to the
// player here directly, without going through AddObjectToContainer.
void TP_MAKE_THISCALL(HookAddInventoryItem, TESObjectREFR, TESBoundObject* apObject, void* apExtra, uint32_t aCount, TESObjectREFR* apOldContainer,
                      const void* apFilter, TESObjectREFR* apOverrideRef)
{
    auto* pPlayer = PlayerCharacter::Get();
    if (apThis == pPlayer && apObject && aCount > 0 && !ScopedInventoryOverride::IsOverriden())
        QueueActorInventoryChange(pPlayer, InventoryChangeEvent(apThis->formID, MakeInventoryEntry(apObject, static_cast<int32_t>(aCount))), apOldContainer);

    TiltedPhoques::ThisCall(s_addInventoryItem, apThis, apObject, apExtra, aCount, apOldContainer, apFilter, apOverrideRef);
}

TP_THIS_FUNCTION(TRemoveItem, BSPointerHandle<TESObjectREFR>*, Actor, BSPointerHandle<TESObjectREFR>*, RemoveItemData*);
TRemoveItem* s_removeItem = nullptr;

BSPointerHandle<TESObjectREFR>* TP_MAKE_THISCALL(HookRemoveItem, Actor, BSPointerHandle<TESObjectREFR>* apResult, RemoveItemData* apData)
{
    if (apData && apData->object && apData->count > 0 && !ScopedInventoryOverride::IsOverriden())
    {
        const bool drop = apData->reason == ITEM_REMOVE_REASON::kDropping;
        // The engine asks for INT_MAX to mean "all of them".
        const auto count = static_cast<int32_t>(std::min<int64_t>(apData->count, apThis->GetItemCountInInventory(apData->object)));
        if (count > 0)
            QueueActorInventoryChange(apThis, InventoryChangeEvent(apThis->formID, MakeInventoryEntry(apData->object, -count), drop), apData->a_otherContainer);
    }

    ScopedEquipOverride _;

    return TiltedPhoques::ThisCall(s_removeItem, apThis, apResult, apData);
}

TP_THIS_FUNCTION(TPickUpObject, void, Actor, TESObjectREFR*, int32_t, bool);
TP_THIS_FUNCTION(TPlayerPickUpObject, bool, Actor, TESObjectREFR*, int32_t, bool);
TPickUpObject* s_pickUpObject = nullptr;
TPlayerPickUpObject* s_playerPickUpObject = nullptr;

// Non temporary objects exist on every client and are removed there through activation sync.
void QueuePickUp(Actor* apActor, TESObjectREFR* apObject, int32_t aCount)
{
    if (!apObject || !apObject->baseForm || aCount <= 0 || ScopedInventoryOverride::IsOverriden())
        return;

    const bool updateClients = apObject->IsTemporary() && !ScopedActivateOverride::IsOverriden();
    QueueActorInventoryChange(apActor, InventoryChangeEvent(apActor->formID, MakeInventoryEntry(apObject->baseForm, aCount), false, updateClients), nullptr);
}

void TP_MAKE_THISCALL(HookPickUpObject, Actor, TESObjectREFR* apObject, int32_t aCount, bool aPlaySounds)
{
    QueuePickUp(apThis, apObject, aCount);
    ScopedInventoryOverride _;
    TiltedPhoques::ThisCall(s_pickUpObject, apThis, apObject, aCount, aPlaySounds);
}

bool TP_MAKE_THISCALL(HookPlayerPickUpObject, Actor, TESObjectREFR* apObject, int32_t aCount, bool aPlaySounds)
{
    QueuePickUp(apThis, apObject, aCount);
    ScopedInventoryOverride _;
    return TiltedPhoques::ThisCall(s_playerPickUpObject, apThis, apObject, aCount, aPlaySounds);
}

TP_THIS_FUNCTION(TUpdateMovementPrimary, void, Actor, float);
TUpdateMovementPrimary* s_updateMovementPrimary = nullptr;

// Remote actors replay the owner's movement, so skip local movement but keep the
// controller step valid for ForcePosition.
void TP_MAKE_THISCALL(HookUpdateMovementPrimary, Actor, float aDeltaTime)
{
    if (apThis->GetExtension()->IsRemote())
    {
        if (apThis->currentProcess)
        {
            if (auto* pController = apThis->currentProcess->GetCharController())
                pController->UpdateStepTiming(aDeltaTime);
        }
        return;
    }

    TiltedPhoques::ThisCall(s_updateMovementPrimary, apThis, aDeltaTime);
}

TP_THIS_FUNCTION(TIsFleeing, bool, Actor);
TIsFleeing* s_isFleeing = nullptr;

bool TP_MAKE_THISCALL(HookIsFleeing, Actor)
{
    if (apThis->GetExtension()->IsPlayer())
        return false;

    return TiltedPhoques::ThisCall(s_isFleeing, apThis);
}

TP_THIS_FUNCTION(TAddDeathItems, void, Actor);
TAddDeathItems* s_addDeathItems = nullptr;

// The owner adds death items and syncs the inventory.
void TP_MAKE_THISCALL(HookAddDeathItems, Actor)
{
    if (apThis->GetExtension()->IsRemote())
        return;

    TiltedPhoques::ThisCall(s_addDeathItems, apThis);
}

TP_THIS_FUNCTION(TProcessResponse, void, AIProcess, void* apVoice, Actor* apTalkingActor, Actor* apTalkedToActor);
TProcessResponse* s_processResponse = nullptr;

void TP_MAKE_THISCALL(HookProcessResponse, AIProcess, void* apVoice, Actor* apTalkingActor, Actor* apTalkedToActor)
{
    if (apTalkingActor && apTalkingActor->GetExtension()->IsRemotePlayer())
        return;

    TiltedPhoques::ThisCall(s_processResponse, apThis, apVoice, apTalkingActor, apTalkedToActor);
}

TP_THIS_FUNCTION(TProcessTracking, void, Actor, float aDelta, NiAVObject* apHead);
TProcessTracking* s_processTracking = nullptr;

// Remote players get their head track target from the owner, see InterpolationSystem.
void TP_MAKE_THISCALL(HookProcessTracking, Actor, float aDelta, NiAVObject* apHead)
{
    if (apThis->GetExtension()->IsRemotePlayer())
        return;

    TiltedPhoques::ThisCall(s_processTracking, apThis, aDelta, apHead);
}

TP_THIS_FUNCTION(TUpdateDetectionState, void, ActorKnowledge, void* apState);
TUpdateDetectionState* s_updateDetectionState = nullptr;

// A remote player's copy must not detect the local player.
void TP_MAKE_THISCALL(HookUpdateDetectionState, ActorKnowledge, void* apState)
{
    auto* pOwner = Cast<Actor>(TESObjectREFR::GetByHandle(apThis->hOwner));
    auto* pTarget = Cast<Actor>(TESObjectREFR::GetByHandle(apThis->hTarget));

    if (pOwner && pTarget && pOwner->GetExtension()->IsRemotePlayer() && pTarget->GetExtension()->IsLocalPlayer())
        return;

    TiltedPhoques::ThisCall(s_updateDetectionState, apThis, apState);
}

// WeaponHandlerHelper, run by the weaponDraw and weaponSheathe animation events
using TWeaponHandler = void(Actor*);
TWeaponHandler* s_weaponDrawHandler = nullptr;
TWeaponHandler* s_weaponSheatheHandler = nullptr;

// The handlers only show or hide the weapon while the actor state is still mid draw or sheathe.
// Remote actors get that state from their owner, usually already past it, so their weapon would
// stay hidden after drawing and visible after sheathing.
void HookWeaponDrawHandler(Actor* apActor)
{
    s_weaponDrawHandler(apActor);

    if (apActor->currentProcess && apActor->GetExtension()->IsRemote())
        apActor->currentProcess->SetWeaponBonesCulled(apActor, false, AIProcess::kWeaponCullWeaponSwitch);
}

void HookWeaponSheatheHandler(Actor* apActor)
{
    s_weaponSheatheHandler(apActor);

    if (apActor->currentProcess && apActor->GetExtension()->IsRemote())
        apActor->currentProcess->SetWeaponBonesCulled(apActor, true, AIProcess::kWeaponCullWeaponSwitch);
}

TiltedPhoques::Initializer s_actorHooks(
    []()
    {
        static VersionDbPtr<TPickUpObject> pickUpObject(2229956);
        static VersionDbPtr<TPlayerPickUpObject> playerPickUpObject(2233019);
        s_pickUpObject = pickUpObject.Get();
        s_playerPickUpObject = playerPickUpObject.Get();
        TP_HOOK(&s_pickUpObject, HookPickUpObject);
        TP_HOOK(&s_playerPickUpObject, HookPlayerPickUpObject);

        static VersionDbPtr<TAddObjectToContainer> addObjectToContainer(2229960);
        s_addObjectToContainer = addObjectToContainer.Get();
        TP_HOOK(&s_addObjectToContainer, HookAddObjectToContainer);

        static VersionDbPtr<TAddInventoryItem> addInventoryItem(2200949);
        s_addInventoryItem = addInventoryItem.Get();
        TP_HOOK(&s_addInventoryItem, HookAddInventoryItem);

        static VersionDbPtr<TRemoveItem> removeItem(2230239);
        s_removeItem = removeItem.Get();
        TP_HOOK(&s_removeItem, HookRemoveItem);

        static VersionDbPtr<TDestructor> destructor(2229565);
        s_destructor = destructor.Get();
        TP_HOOK(&s_destructor, HookDestructor);

        static VersionDbPtr<TModActorValue> modActorValue(2230987);
        s_modActorValue = modActorValue.Get();
        TP_HOOK(&s_modActorValue, HookModActorValue);

        static VersionDbPtr<TUpdateMovementPrimary> updateMovementPrimary(2229759);
        s_updateMovementPrimary = updateMovementPrimary.Get();
        TP_HOOK(&s_updateMovementPrimary, HookUpdateMovementPrimary);

        static VersionDbPtr<TIsFleeing> isFleeing(2230011);
        s_isFleeing = isFleeing.Get();
        TP_HOOK(&s_isFleeing, HookIsFleeing);

        static VersionDbPtr<TAddDeathItems> addDeathItems(2229588);
        s_addDeathItems = addDeathItems.Get();
        TP_HOOK(&s_addDeathItems, HookAddDeathItems);

        static VersionDbPtr<TProcessResponse> processResponse(2232103);
        s_processResponse = processResponse.Get();
        TP_HOOK(&s_processResponse, HookProcessResponse);

        // Actor vtable slot 301, PlayerCharacter does not override it
        static VersionDbPtr<TProcessTracking> processTracking(2230488);
        s_processTracking = processTracking.Get();
        TP_HOOK(&s_processTracking, HookProcessTracking);

        static VersionDbPtr<TUpdateDetectionState> updateDetectionState(2235119);
        s_updateDetectionState = updateDetectionState.Get();
        TP_HOOK(&s_updateDetectionState, HookUpdateDetectionState);

        static VersionDbPtr<TWeaponHandler> weaponDrawHandler(2229545);
        static VersionDbPtr<TWeaponHandler> weaponSheatheHandler(2229546);
        s_weaponDrawHandler = weaponDrawHandler.Get();
        s_weaponSheatheHandler = weaponSheatheHandler.Get();
        TP_HOOK(&s_weaponDrawHandler, HookWeaponDrawHandler);
        TP_HOOK(&s_weaponSheatheHandler, HookWeaponSheatheHandler);
    });
}

bool Actor::IsDead() const noexcept
{
    // Papyrus Actor.IsDead calls the virtual with abNotEssential set.
    return static_cast<const TESObjectREFR*>(this)->IsDead(true);
}

bool Actor::IsDragon() const noexcept
{
    return false;
}

bool Actor::IsPlayerSummon() const noexcept
{
    const Actor* pCommandingActor = GetCommandingActor();
    return pCommandingActor && pCommandingActor->formID == 0x14;
}

Actor* Actor::GetCommandingActor() const noexcept
{
    if (currentProcess && currentProcess->middleProcess && currentProcess->middleProcess->commandingActor)
        return Cast<Actor>(TESObjectREFR::GetByHandle(currentProcess->middleProcess->commandingActor.handle.iBits));
    return nullptr;
}

void Actor::SetCommandingActor(BSPointerHandle<TESObjectREFR> aCommandingActor) noexcept
{
    if (currentProcess && currentProcess->middleProcess)
    {
        currentProcess->middleProcess->commandingActor = aCommandingActor;
        boolFlags |= ActorFlags::IS_COMMANDED_ACTOR;
    }
}

bool Actor::InitiateMountPackage(Actor*) noexcept
{
    // Fallout 4 has no rideable mounts.
    return false;
}

bool Actor::RemoveSpell(MagicItem* apSpell) noexcept
{
    if (!apSpell)
        return false;
    TP_THIS_FUNCTION(TRemoveSpell, bool, Actor, MagicItem*);
    static VersionDbPtr<TRemoveSpell> removeSpell(2231253);
    return TiltedPhoques::ThisCall(removeSpell, this, apSpell);
}

void Actor::ForcePosition(const NiPoint3& acPosition) noexcept
{
    ScopedReferencesOverride recursionGuard;

    bool updateController = true;
    if (GetExtension()->IsRemote() && currentProcess)
    {
        // A new controller can be positioned before its first movement update, without a step
        if (auto* pController = currentProcess->GetCharController())
            updateController = pController->UpdateStepTiming();
    }

    SetPosition(acPosition, updateController);
}

void Actor::Kill() noexcept
{
    // Never kill players
    if (GetExtension()->IsPlayer())
        return;
    KillImpl(nullptr, 100.f, true, true);
}

void Actor::Respawn() noexcept
{
    Resurrect(false, true);
}

void Actor::SetPackage(TESPackage* apPackage) noexcept
{
    PutCreatedPackage(apPackage, false, true, true);
}

void Actor::SetEssentialEx(bool aSet) noexcept
{
    SetEssential(aSet);
    if (auto* pBase = Cast<TESNPC>(baseForm))
        pBase->actorData.SetEssential(aSet);
}

void Actor::SetNoBleedoutRecovery(bool aSet) noexcept
{
    TP_THIS_FUNCTION(TSetNoBleedoutRecovery, void, Actor, bool);
    static VersionDbPtr<TSetNoBleedoutRecovery> setNoBleedoutRecovery(2231052);
    TiltedPhoques::ThisCall(setNoBleedoutRecovery, this, aSet);
}

void Actor::SetFactionRank(const TESFaction* apFaction, int8_t aRank) noexcept
{
    TP_THIS_FUNCTION(TSetFactionRank, void, Actor, const TESFaction*, int8_t);
    static VersionDbPtr<TSetFactionRank> setFactionRank(2230101);
    TiltedPhoques::ThisCall(setFactionRank, this, apFaction, aRank);
}

void Actor::SetPlayerRespawnMode(bool aSet) noexcept
{
    SetEssentialEx(aSet);
    // Makes the player go in an unrecoverable bleedout state
    SetNoBleedoutRecovery(aSet);

    if (formID != 0x14)
    {
        // PlayerFaction
        if (auto* pPlayerFaction = Cast<TESFaction>(TESForm::GetById(0x1C21C)))
            SetFactionRank(pPlayerFaction, 1);
    }
}

void Actor::SetWeaponDrawnEx(bool aDraw) noexcept
{
    if (GetActorState()->IsWeaponDrawn() == aDraw)
        return;
    DrawWeaponMagicHands(aDraw);
}

namespace
{
struct BSSoundHandle
{
    uint32_t soundId = 0xFFFFFFFF;
    bool assumeSuccess = false;
    int8_t state = 0;
};
static_assert(sizeof(BSSoundHandle) == 0x8);

TP_THIS_FUNCTION(TSpeakSoundHelper, void, TESObjectREFR, const char* apFile, BSSoundHandle& aHandle, void* apOutputModel, void* apTopic);
TSpeakSoundHelper* s_speakSoundHelper = nullptr;

// Every line an actor voices goes through here, menu dialogue included.
void TP_MAKE_THISCALL(HookSpeakSoundHelper, TESObjectREFR, const char* apFile, BSSoundHandle& aHandle, void* apOutputModel, void* apTopic)
{
    if (auto* pActor = Cast<Actor>(apThis); pActor && apFile)
    {
        // The player having the conversation may not own the NPC. Ambient speech only comes from the actor's owner.
        const bool cIsPlayerDialogue = MenuTopicManager::IsPlayerDialogueSpeaker(pActor);
        if (cIsPlayerDialogue || pActor->GetExtension()->IsLocal())
        {
            // Clients that play the same scene skip the line, unless their copy is stuck in an earlier phase
            BGSScene* pScene = cIsPlayerDialogue ? nullptr : pActor->GetCurrentScene();
            uint32_t sceneId = 0;
            uint32_t scenePhase = 0;
            if (pScene && pScene->IsPlaying())
            {
                sceneId = pScene->formID;
                scenePhase = pScene->currentPhaseIndex;
            }

            World::Get().GetRunner().Trigger(DialogueEvent(pActor->formID, apFile, sceneId, scenePhase));
        }
    }

    TiltedPhoques::ThisCall(s_speakSoundHelper, apThis, apFile, aHandle, apOutputModel, apTopic);
}

TiltedPhoques::Initializer s_speakSoundHooks(
    []()
    {
        static VersionDbPtr<TSpeakSoundHelper> speakSoundHelper(2201212);
        s_speakSoundHelper = speakSoundHelper.Get();
        TP_HOOK(&s_speakSoundHelper, HookSpeakSoundHelper);
    });
} // namespace

void Actor::SpeakSound(const char* apFile)
{
    BSSoundHandle handle{};
    TiltedPhoques::ThisCall(s_speakSoundHelper, this, apFile, handle, nullptr, nullptr);
}

void Actor::GenerateMagicCasters() noexcept
{
    using CS = MagicSystem::CastingSource;

    for (int i = 0; i < 4; i++)
    {
        if (casters[i] == nullptr)
            casters[i] = Cast<ActorMagicCaster>(GetMagicCaster(static_cast<CS>(i)));
    }
}

TESForm* Actor::GetEquippedWeapon(uint32_t aSlotId) const noexcept
{
    if (!currentProcess || !currentProcess->middleProcess)
        return nullptr;

    for (const auto& item : currentProcess->middleProcess->equippedItems)
    {
        if (item.equipIndex == aSlotId && item.object && item.object->formType == FormType::Weapon)
            return item.object;
    }
    return nullptr;
}

TESNPC* Actor::GetLeveledPick() const noexcept
{
    // Fallout 4 keeps one template per template-use flag. The first one set (traits when used) decides
    // the identity.
    const ExtraDataList* pExtraList = extraData;
    const auto* pExtra = pExtraList ? static_cast<ExtraLeveledCreature*>(pExtraList->GetByType(ExtraDataType::LeveledCreature)) : nullptr;
    if (!pExtra)
        return nullptr;

    for (TESActorBase* pTemplate : pExtra->templateBases)
    {
        if (!pTemplate)
            continue;
        if (pTemplate->formType != FormType::Npc || pTemplate->IsTemporary())
            return nullptr;
        return static_cast<TESNPC*>(pTemplate);
    }

    return nullptr;
}

Factions Actor::GetFactions() const noexcept
{
    Factions result;

    auto& modSystem = World::Get().GetModSystem();

    if (auto* pNpc = Cast<TESNPC>(baseForm))
    {
        for (const auto& entry : pNpc->actorData.factions)
        {
            Faction faction;
            modSystem.GetServerModId(entry.faction->formID, faction.Id);
            faction.Rank = entry.rank;
            result.NpcFactions.push_back(faction);
        }
    }

    const ExtraDataList* pExtraList = extraData;
    if (auto* pChanges = pExtraList ? static_cast<ExtraFactionChanges*>(pExtraList->GetByType(ExtraDataType::Faction)) : nullptr)
    {
        for (const auto& entry : pChanges->entries)
        {
            Faction faction;
            modSystem.GetServerModId(entry.faction->formID, faction.Id);
            faction.Rank = entry.rank;
            result.ExtraFactions.push_back(faction);
        }
    }

    return result;
}

void Actor::RemoveFromAllFactions() noexcept
{
    // A rank of -1 expels the actor from the faction.
    Factions factions = GetFactions();
    auto& modSystem = World::Get().GetModSystem();
    for (const auto* pList : {&factions.NpcFactions, &factions.ExtraFactions})
    {
        for (const auto& entry : *pList)
        {
            if (auto* pFaction = Cast<TESFaction>(TESForm::GetById(modSystem.GetGameId(entry.Id))))
                SetFactionRank(pFaction, -1);
        }
    }
}

void Actor::SetFactions(const Factions& acFactions) noexcept
{
    RemoveFromAllFactions();

    auto& modSystem = World::Get().GetModSystem();
    for (const auto* pList : {&acFactions.NpcFactions, &acFactions.ExtraFactions})
    {
        for (const auto& entry : *pList)
        {
            if (auto* pFaction = Cast<TESFaction>(TESForm::GetById(modSystem.GetGameId(entry.Id))))
                SetFactionRank(pFaction, entry.Rank);
        }
    }
}

void Actor::PickUpObject(TESObjectREFR* apObject, int32_t aCount, bool aPlaySounds, float) noexcept
{
    TP_THIS_FUNCTION(TPickUpObject, void, Actor, TESObjectREFR*, int32_t, bool);
    static VersionDbPtr<TPickUpObject> pickUpObject(2229956);
    TiltedPhoques::ThisCall(pickUpObject, this, apObject, aCount, aPlaySounds);
}

void Actor::UnEquipAll() noexcept
{
    TP_THIS_FUNCTION(TUnequipAll, void, Actor);
    static VersionDbPtr<TUnequipAll> unequipAll(2229944);
    TiltedPhoques::ThisCall(unequipAll, this);
}

Inventory Actor::GetActorInventory() const noexcept
{
    // Fallout 4 actors have no equipped spells or shouts to sync.
    return GetInventory();
}

Inventory Actor::GetEquipment() const noexcept
{
    Inventory inventory = GetInventory();
    inventory.RemoveByFilter([](const auto& entry) { return !entry.IsWorn(); });
    return inventory;
}

void Actor::SetActorInventory(const Inventory& acInventory) noexcept
{
    spdlog::info("Setting inventory for actor {:X}", formID);

    Inventory currentInventory = GetActorInventory();

    if (!GetExtension()->IsPlayer() && currentInventory.ContainsQuestItems())
        SetInventoryRetainingQuestItems(currentInventory, acInventory);
    else
        SetInventory(acInventory);
}

void Actor::DropOrPickUpObject(const Inventory::Entry& arEntry, NiPoint3* apLocation, NiPoint3* apRotation) noexcept
{
    auto& modSystem = World::Get().GetModSystem();

    auto* pObject = Cast<TESBoundObject>(TESForm::GetById(modSystem.GetGameId(arEntry.BaseId)));
    if (!pObject)
    {
        spdlog::warn("Object to drop not found, {:X}:{:X}.", arEntry.BaseId.ModId, arEntry.BaseId.BaseId);
        return;
    }

    // TODO: pick up
    if (arEntry.Count >= 0)
        return;

    RemoveItemData data{};
    *reinterpret_cast<uint32_t*>(data.stackData) = 0x80000000;
    data.object = pObject;
    data.count = -arEntry.Count;
    data.reason = ITEM_REMOVE_REASON::kDropping;
    data.dropLoc = apLocation;
    data.rotate = apRotation;
    RemoveItem(data);
}

Inventory Actor::GetPowerArmorInventory() const noexcept
{
    using TGetKeyword = BGSKeyword*();
    static VersionDbPtr<TGetKeyword> getKeyword(2194743);
    static VersionDbPtr<TGetKeyword> getFrameKeyword(2194744);
    auto* pKeyword = getKeyword.Get()();
    auto* pFrameKeyword = getFrameKeyword.Get()();
    return GetInventory([pKeyword, pFrameKeyword](TESForm& aForm)
    {
        auto* pKeywords = aForm.formType == FormType::Armor ? Cast<BGSKeywordForm>(&aForm) : nullptr;
        return pKeyword && pKeywords && pKeywords->Contains(pKeyword) && (!pFrameKeyword || !pKeywords->Contains(pFrameKeyword));
    }, true);
}

void Actor::ApplyPowerArmorInventory(const Inventory& acInventory) noexcept
{
    ScopedInventoryOverride inventoryOverride;
    ScopedEquipOverride equipOverride;
    auto& modSystem = World::Get().GetModSystem();
    auto* pEquip = EquipManager::Get();
    auto current = GetPowerArmorInventory();
    Vector<GameId> reconciled;
    for (const auto& entry : current.Entries)
    {
        if (std::find(reconciled.begin(), reconciled.end(), entry.BaseId) != reconciled.end())
            continue;
        reconciled.push_back(entry.BaseId);
        int32_t count = 0;
        int32_t wanted = 0;
        for (const auto& item : current.Entries)
            if (item.BaseId == entry.BaseId)
                count += item.Count;
        for (const auto& item : acInventory.Entries)
            if (item.BaseId == entry.BaseId)
                wanted += item.Count;
        if (count != wanted)
        {
            auto removed = entry;
            removed.Count = -count;
            AddOrRemoveItem(removed);
        }
    }
    current = GetPowerArmorInventory();
    for (const auto& entry : acInventory.Entries)
    {
        auto* pObject = Cast<TESBoundObject>(TESForm::GetById(modSystem.GetGameId(entry.BaseId)));
        if (!pObject || pObject->formType != FormType::Armor)
            continue;
        const auto it = std::find_if(current.Entries.begin(), current.Entries.end(), [&entry](const auto& aItem)
        { return aItem.BaseId == entry.BaseId; });
        const bool changed = it == current.Entries.end() || it->Mods != entry.Mods || it->ExtraHealth != entry.ExtraHealth;
        if (it != current.Entries.end() && it->IsWorn() && (changed || !entry.IsWorn()))
            pEquip->UnEquip(this, pObject, nullptr, 1, nullptr, false, true, false, false, nullptr);
        if (it == current.Entries.end())
        {
            auto item = entry;
            item.ExtraWorn = false;
            AddOrRemoveItem(item);
        }
        if (it == current.Entries.end() || it->Mods != entry.Mods)
            SetItemMods(pObject, entry.Mods);
        auto condition = entry;
        if (changed)
            condition.ExtraWorn = false;
        SetInventoryItemCondition(condition);
        if (entry.IsWorn() && (changed || !it->IsWorn()))
            pEquip->Equip(this, pObject, nullptr, 1, nullptr, false, true, false, false);
    }
}

bool Actor::IsInPowerArmor() const noexcept
{
    using TQActorInPowerArmor = bool(const Actor*);
    static VersionDbPtr<TQActorInPowerArmor> qActorInPowerArmor(2219437);
    return qActorInPowerArmor.Get()(this);
}

TESObjectREFR* Actor::GetPowerArmorFurniture() const noexcept
{
    using TGetArmorFurnitureHandle = uint32_t*(uint32_t*, const Actor*);
    static VersionDbPtr<TGetArmorFurnitureHandle> getArmorFurnitureHandle(2219423);
    uint32_t handle = 0;
    getArmorFurnitureHandle.Get()(&handle, this);
    return handle ? TESObjectREFR::GetByHandle(handle) : nullptr;
}

// Instant switch used by the Papyrus Actor.SwitchToPowerArmor.
void Actor::EnterPowerArmor(TESObjectREFR* apFurniture) noexcept
{
    using TSwitchToPowerArmor = void(Actor*, TESObjectREFR*, bool);
    static VersionDbPtr<TSwitchToPowerArmor> switchToPowerArmor(2219442);
    if (apFurniture)
        switchToPowerArmor.Get()(this, apFurniture, false);
}

void Actor::ExitPowerArmor() noexcept
{
    using TSwitchFromPowerArmor = bool(Actor*);
    static VersionDbPtr<TSwitchFromPowerArmor> switchFromPowerArmor(2252033);
    switchFromPowerArmor.Get()(this);
}
