#include <TiltedOnlinePCH.h>

#include <TESObjectREFR.h>
#include <Forms/TESObjectCELL.h>
#include <Actor.h>
#include <PlayerCharacter.h>
#include <EquipManager.h>
#include <Games/Overrides.h>
#include <World.h>
#include <Games/Misc/Lock.h>
#include <Events/ActivateEvent.h>
#include <Events/LockChangeEvent.h>
#include <Events/InventoryChangeEvent.h>
#include <cmath>

TESObjectREFR* TESObjectREFR::GetByHandle(uint32_t aHandle) noexcept
{
    using TGetReference = bool(const uint32_t&, TESObjectREFR*&);
    static VersionDbPtr<TGetReference> getReference(2188681);
    TESObjectREFR* pReference = nullptr;
    getReference.Get()(aHandle, pReference);
    if (pReference)
        pReference->handleRefObject.DecRefHandle();
    return pReference;
}

BSPointerHandle<TESObjectREFR> TESObjectREFR::GetHandle() const noexcept
{
    TP_THIS_FUNCTION(TGetHandle, BSPointerHandle<TESObjectREFR>*, const TESObjectREFR, BSPointerHandle<TESObjectREFR>*);
    static VersionDbPtr<TGetHandle> getHandle(2201196);
    BSPointerHandle<TESObjectREFR> handle;
    TiltedPhoques::ThisCall(getHandle, this, &handle);
    return handle;
}

uint32_t* TESObjectREFR::GetNullHandle() noexcept
{
    static VersionDbPtr<uint32_t> nullHandle(4795988);
    return nullHandle.Get();
}

TESObjectCELL* TESObjectREFR::GetParentCellEx() const noexcept
{
    return parentCell ? parentCell : GetSaveParentCell();
}

uint32_t TESObjectREFR::GetCellId() const noexcept
{
    const auto* pCell = GetParentCellEx();
    return pCell ? pCell->formID : 0;
}

TESWorldSpace* TESObjectREFR::GetWorldSpace() const noexcept
{
    const auto* pCell = GetParentCellEx();
    return pCell && !(pCell->cellFlags & 1) ? pCell->worldspace : nullptr;
}

ExtraDataList* TESObjectREFR::GetExtraDataList() noexcept
{
    return extraData.object;
}

Lock* TESObjectREFR::GetLock() const noexcept
{
    TP_THIS_FUNCTION(TGetLock, Lock*, const TESObjectREFR);
    static VersionDbPtr<TGetLock> getLock(2202648);
    return TiltedPhoques::ThisCall(getLock, this);
}

TESContainer* TESObjectREFR::GetContainer() const noexcept
{
    TP_THIS_FUNCTION(TGetContainer, TESContainer*, const TESObjectREFR);
    static VersionDbPtr<TGetContainer> getContainer(2201022);
    return TiltedPhoques::ThisCall(getContainer, this);
}

int64_t TESObjectREFR::GetItemCountInInventory(TESForm* apItem) const noexcept
{
    TP_THIS_FUNCTION(TGetCount, bool, const TESObjectREFR, uint32_t&, TESForm*, bool);
    static VersionDbPtr<TGetCount> getCount(2200996);
    uint32_t count = 0;
    TiltedPhoques::ThisCall(getCount, this, count, apItem, false);
    return count;
}

void TESObjectREFR::Enable() const noexcept
{
    TP_THIS_FUNCTION(TEnable, void, const TESObjectREFR, bool);
    static VersionDbPtr<TEnable> enable(2201150);
    TiltedPhoques::ThisCall(enable, this, false);
}

void TESObjectREFR::SetRotation(float aX, float aY, float aZ) noexcept
{
    TP_THIS_FUNCTION(TSetAngle, void, TESObjectREFR, const NiPoint3&);
    static VersionDbPtr<TSetAngle> setAngle(2201134);
    // An actor's pitch is where it looks; as a reference angle it tilts the whole body.
    const bool isActor = formType == FormType::Character;
    const NiPoint3 angle(glm::vec3(isActor ? 0.f : aX, isActor ? 0.f : aY, aZ));
    TiltedPhoques::ThisCall(setAngle, this, angle);
}

const float TESObjectREFR::GetHeight() noexcept
{
    return GetBoundMax().z - GetBoundMin().z;
}

namespace
{
struct ScopedReadLock
{
    explicit ScopedReadLock(BSReadWriteLock& aLock) noexcept
        : m_lock(aLock)
    {
        TP_THIS_FUNCTION(TLockForRead, void, BSReadWriteLock);
        static VersionDbPtr<TLockForRead> lockForRead(2267897);
        TiltedPhoques::ThisCall(lockForRead, &m_lock);
    }

    ~ScopedReadLock() noexcept
    {
        TP_THIS_FUNCTION(TUnlockRead, void, BSReadWriteLock);
        static VersionDbPtr<TUnlockRead> unlockRead(2267903);
        TiltedPhoques::ThisCall(unlockRead, &m_lock);
    }

    BSReadWriteLock& m_lock;
};
} // namespace

void TESObjectREFR::EnableImpl() noexcept
{
    Enable();
}

// Same steps as the disable and markfordelete console commands.
void TESObjectREFR::Delete() const noexcept
{
    auto* pThis = const_cast<TESObjectREFR*>(this);
    if (IsDeleted() || pThis == PlayerCharacter::Get())
        return;

    if (!IsDisabled())
        pThis->DisableImpl();
    pThis->SetDelete(true);

    if (IsTemporary() && (TESForm::flags & PERSISTENT) == 0)
    {
        TP_THIS_FUNCTION(TQueueDeletion, void, TESObjectREFR, bool);
        static VersionDbPtr<TQueueDeletion> queueDeletion(2201199);
        TiltedPhoques::ThisCall(queueDeletion, pThis, true);
    }
}

TP_THIS_FUNCTION(TActivateRef, bool, TESObjectREFR, TESObjectREFR*, TESBoundObject*, int32_t, bool, bool, bool);
static TActivateRef* RealActivateRef = nullptr;

bool TESObjectREFR::Activate(TESObjectREFR* apActivator, uint8_t, TESBoundObject* apObjectToGet, int32_t aCount, char aDefaultProcessing) noexcept
{
    ScopedActivateOverride _;
    return TiltedPhoques::ThisCall(RealActivateRef, this, apActivator, apObjectToGet, aCount, aDefaultProcessing != 0, false, false);
}

bool TESObjectREFR::PlayAnimation(BSFixedString* apEventName) noexcept
{
    // Papyrus ObjectReference.PlayAnimation notifies the animation graph directly.
    return animationGraphHolder.SendAnimationEvent(apEventName);
}

bool TESObjectREFR::PlayAnimationAndWait(BSFixedString*, BSFixedString* apEventName) noexcept
{
    return PlayAnimation(apEventName);
}

TESObjectREFR::OpenState TESObjectREFR::GetOpenState() noexcept
{
    using TGetOpenState = OpenState(const TESObjectREFR*);
    static VersionDbPtr<TGetOpenState> getOpenState(2192799);
    return getOpenState.Get()(this);
}

Lock* TESObjectREFR::CreateLock() noexcept
{
    TP_THIS_FUNCTION(TAddLock, Lock*, TESObjectREFR);
    static VersionDbPtr<TAddLock> addLock(2202646);
    return TiltedPhoques::ThisCall(addLock, this);
}

TP_THIS_FUNCTION(TAddLockChange, void, TESObjectREFR);
static TAddLockChange* RealAddLockChange = nullptr;

void TESObjectREFR::LockChange() noexcept
{
    TiltedPhoques::ThisCall(RealAddLockChange, this);
}

void TESObjectREFR::MoveTo(TESObjectCELL* apCell, const NiPoint3& acPosition) const noexcept
{
    ScopedReferencesOverride recursionGuard;

    auto* pActor = Cast<Actor>(const_cast<TESObjectREFR*>(this));
    if (!pActor || !apCell)
    {
        spdlog::warn("TESObjectREFR::MoveTo only supports actors on Fallout 4, form id {:X}", formID);
        return;
    }

    const bool isInterior = apCell->cellFlags & 1;
    TESWorldSpace* pWorldSpace = isInterior ? nullptr : apCell->worldspace;

    // Mirrors Papyrus MoveTo: the player is queued through RequestPositionPlayer,
    // other actors are warped.
    if (auto* pPlayer = Cast<PlayerCharacter>(pActor); pPlayer && pPlayer == PlayerCharacter::Get())
    {
        struct PlayerTargetLocation
        {
            TESWorldSpace* world;        // 00
            TESObjectCELL* interior;     // 08
            void* transitionTeleport;    // 10
            NiPoint3 location;           // 18
            NiPoint3 angle;              // 24
            TESObjectREFR* walkThroughDoor; // 30
            void* arrivalFunc;           // 38
            int64_t arrivalFuncData;     // 40
            uint32_t furnitureRef;       // 48
            uint32_t fastTravelMarker;   // 4C
            float fastTravelDistance;    // 50
            bool resetWeather;           // 54
            bool allowAutoSave;          // 55
            bool preventLoadMenu;        // 56
            bool skyTransition;          // 57
            bool isValid;                // 58
        };
        static_assert(sizeof(PlayerTargetLocation) == 0x60);

        PlayerTargetLocation target{};
        target.world = pWorldSpace;
        target.interior = isInterior ? apCell : nullptr;
        target.location = acPosition;
        target.angle.z = rotation.z;
        target.furnitureRef = *GetNullHandle();
        target.fastTravelMarker = *GetNullHandle();
        target.resetWeather = true;
        target.allowAutoSave = true;
        target.isValid = true;

        TP_THIS_FUNCTION(TRequestPositionPlayer, void, PlayerCharacter, const PlayerTargetLocation&);
        static VersionDbPtr<TRequestPositionPlayer> requestPositionPlayer(2232913);
        TiltedPhoques::ThisCall(requestPositionPlayer, pPlayer, target);
        return;
    }

    TP_THIS_FUNCTION(TWarpTo, void, Actor, const NiPoint3&, float, float, TESObjectCELL*, TESWorldSpace*, bool, bool, bool);
    static VersionDbPtr<TWarpTo> warpTo(2229712);
    TiltedPhoques::ThisCall(warpTo, pActor, acPosition, rotation.z, 0.f, apCell, pWorldSpace, true, false, true);
}

Inventory TESObjectREFR::GetInventory() const noexcept
{
    return GetInventory([](TESForm&) { return true; });
}

Inventory TESObjectREFR::GetInventory(std::function<bool(TESForm&)> aFilter, bool aIncludeCondition) const noexcept
{
    Inventory inventory;
    if (!inventoryList)
        return inventory;

    auto& modSystem = World::Get().GetModSystem();

    ScopedReadLock _{inventoryList->lock};
    for (const auto& item : inventoryList->items)
    {
        if (!item.object || !aFilter(*item.object))
            continue;

        for (auto* pStack = item.stack; pStack; pStack = pStack->next)
        {
            Inventory::Entry entry{};
            modSystem.GetServerModId(item.object->formID, entry.BaseId);
            entry.Count = static_cast<int32_t>(pStack->count);
            entry.ExtraWorn = pStack->IsEquipped();
            if (aIncludeCondition)
            {
                const float health = pStack->extra ? pStack->extra->GetHealthPercent() : -1.f;
                entry.ExtraHealth = health < 0.f ? 1.f : std::round(std::clamp(health, 0.f, 1.f) * 1000.f) / 1000.f;
            }
            for (const uint32_t modId : GetStackMods(pStack))
                modSystem.GetServerModId(modId, entry.Mods.emplace_back());
            inventory.Entries.push_back(std::move(entry));
        }
    }

    return inventory;
}

Inventory TESObjectREFR::GetArmor() const noexcept
{
    return GetInventory([](TESForm& aForm) { return aForm.formType == FormType::Armor; });
}

Inventory TESObjectREFR::GetWornArmor() const noexcept
{
    Inventory wornArmor = GetArmor();
    wornArmor.RemoveByFilter([](const auto& entry) { return !entry.IsWorn(); });
    return wornArmor;
}

void TESObjectREFR::RemoveAllItems() noexcept
{
    ScopedEquipOverride equipOverride;

    TP_THIS_FUNCTION(TRemoveAllItems, void, TESObjectREFR, TESObjectREFR*, ITEM_REMOVE_REASON);
    static VersionDbPtr<TRemoveAllItems> removeAllItems(2200985);
    TiltedPhoques::ThisCall(removeAllItems, this, nullptr, ITEM_REMOVE_REASON::kRemove);
}

Vector<uint32_t> TESObjectREFR::RemoveNonQuestItems(Inventory& aCurrentInventory) noexcept
{
    ScopedEquipOverride equipOverride;

    Vector<uint32_t> questEntries{};
    auto& modSystem = World::Get().GetModSystem();

    for (auto& entry : aCurrentInventory.Entries)
    {
        if (entry.IsQuestItem)
        {
            questEntries.emplace_back(modSystem.GetGameId(entry.BaseId));
            continue;
        }

        if (entry.Count <= 0)
            continue;

        entry.Count = -entry.Count;
        AddOrRemoveItem(entry, true);
    }

    return questEntries;
}

void TESObjectREFR::SetInventory(const Inventory& aInventory) noexcept
{
    ScopedInventoryOverride _;

    RemoveAllItems();

    for (const Inventory::Entry& entry : aInventory.Entries)
    {
        if (entry.Count != 0)
            AddOrRemoveItem(entry, true);
    }
}

void TESObjectREFR::SetInventoryRetainingQuestItems(Inventory& aCurrentInventory, const Inventory& acSourceInventory) noexcept
{
    ScopedInventoryOverride _;

    Vector<uint32_t> questItemIds = RemoveNonQuestItems(aCurrentInventory);
    auto& modSystem = World::Get().GetModSystem();

    for (const auto& entry : acSourceInventory.Entries)
    {
        const uint32_t gameId = modSystem.GetGameId(entry.BaseId);
        if (entry.Count != 0 && std::find(questItemIds.begin(), questItemIds.end(), gameId) == questItemIds.end())
            AddOrRemoveItem(entry, true);
    }
}

void TESObjectREFR::AddOrRemoveItem(const Inventory::Entry& arEntry, bool) noexcept
{
    auto& modSystem = World::Get().GetModSystem();

    const uint32_t objectId = modSystem.GetGameId(arEntry.BaseId);
    auto* pObject = Cast<TESBoundObject>(TESForm::GetById(objectId));
    if (!pObject)
    {
        spdlog::warn("{}: Object to add not found, {:X}:{:X}.", __FUNCTION__, arEntry.BaseId.ModId, arEntry.BaseId.BaseId);
        return;
    }

    if (arEntry.Count > 0)
    {
        ExtraDataList* pExtra = nullptr;
        AddObjectToContainer(pObject, &pExtra, arEntry.Count, nullptr, ITEM_REMOVE_REASON::kRemove);
        AttachItemMods(pObject, arEntry.Mods);

        if (arEntry.IsWorn())
        {
            if (auto* pActor = Cast<Actor>(this))
                EquipManager::Get()->Equip(pActor, pObject, nullptr, arEntry.Count, nullptr, false, true, false, false);
        }
    }
    else if (arEntry.Count < 0)
    {
        RemoveItemData data{};
        // BSTSmallArray<uint32_t, 4> with local storage and no stack ids.
        *reinterpret_cast<uint32_t*>(data.stackData) = 0x80000000;
        data.object = pObject;
        data.count = -arEntry.Count;
        data.reason = ITEM_REMOVE_REASON::kRemove;
        RemoveItem(data);
    }
}

void TESObjectREFR::SetInventoryItemCondition(const Inventory::Entry& acEntry) noexcept
{
    if (!inventoryList)
        return;
    auto* pItem = TESForm::GetById(World::Get().GetModSystem().GetGameId(acEntry.BaseId));
    TP_THIS_FUNCTION(TLock, void, BSReadWriteLock);
    static VersionDbPtr<TLock> lock(2267898);
    static VersionDbPtr<TLock> unlock(2267904);
    TiltedPhoques::ThisCall(lock.Get(), &inventoryList->lock);
    for (auto& item : inventoryList->items)
    {
        if (item.object != pItem)
            continue;
        for (auto* pStack = item.stack; pStack; pStack = pStack->next)
        {
            if (pStack->IsEquipped() != acEntry.IsWorn())
                continue;
            Vector<GameId> mods;
            for (const auto id : GetStackMods(pStack))
                World::Get().GetModSystem().GetServerModId(id, mods.emplace_back());
            if (mods != acEntry.Mods)
                continue;
            if (!pStack->extra && acEntry.ExtraHealth < 1.f)
            {
                pStack->extra = ExtraDataList::New();
                if (pStack->extra)
                    pStack->extra->refCount = 1;
            }
            if (pStack->extra)
                pStack->extra->SetHealthPercent(acEntry.ExtraHealth);
            break;
        }
        break;
    }
    TiltedPhoques::ThisCall(unlock.Get(), &inventoryList->lock);
}

void TESObjectREFR::SetItemMods(TESBoundObject* apItem, const Vector<GameId>& acMods) noexcept
{
    if (!apItem || GetItemCountInInventory(apItem) != 1)
        return;
    using TModifyInventoryItemMod = bool(void*, uint32_t, TESObjectREFR*, TESBoundObject*, TESForm*, bool);
    static VersionDbPtr<TModifyInventoryItemMod> modify(2254249);
    const auto current = GetItemMods(apItem, 0);
    auto& modSystem = World::Get().GetModSystem();
    ScopedInventoryOverride inventoryOverride;
    ScopedEquipOverride equipOverride;
    for (const auto id : current)
    {
        GameId modId{};
        if (modSystem.GetServerModId(id, modId) && std::find(acMods.begin(), acMods.end(), modId) == acMods.end())
        {
            if (auto* pMod = TESForm::GetById(id))
                modify.Get()(nullptr, 0, this, apItem, pMod, false);
        }
    }
    AttachItemMods(apItem, acMods);
}

Vector<uint32_t> TESObjectREFR::GetStackMods(const BGSInventoryItem::Stack* apStack) noexcept
{
    // BGSObjectInstanceExtra: a buffer of BGSMod::ObjectIndexData { formId, index, rank, disabled }.
    struct ObjectIndexData
    {
        uint32_t formId;
        uint8_t index;
        uint8_t rank;
        uint8_t disabled;
    };
    struct DataBuffer
    {
        const ObjectIndexData* data;
        uint32_t size;
    };

    Vector<uint32_t> mods;
    if (!apStack || !apStack->extra)
        return mods;

    const auto* pInstance = apStack->extra->GetByType(ExtraDataType::ObjectInstance);
    if (!pInstance)
        return mods;

    const auto* pValues = *reinterpret_cast<DataBuffer* const*>(reinterpret_cast<const uint8_t*>(pInstance) + 0x18);
    if (!pValues || !pValues->data)
        return mods;

    for (uint32_t i = 0; i < pValues->size / sizeof(ObjectIndexData); ++i)
    {
        if (!pValues->data[i].disabled)
            mods.push_back(pValues->data[i].formId);
    }
    return mods;
}

Vector<uint32_t> TESObjectREFR::GetItemMods(const TESForm* apItem, uint32_t aStackId) const noexcept
{
    if (!inventoryList)
        return {};

    ScopedReadLock _{inventoryList->lock};
    for (const auto& item : inventoryList->items)
    {
        if (item.object != apItem)
            continue;

        auto* pStack = item.stack;
        for (uint32_t i = 0; pStack && i < aStackId; ++i)
            pStack = pStack->next;
        return GetStackMods(pStack);
    }
    return {};
}

// Same steps as Papyrus ObjectReference.AttachModToInventoryItem, which only mods singular items.
void TESObjectREFR::AttachItemMods(TESBoundObject* apItem, const Vector<GameId>& acMods) noexcept
{
    if (!apItem || acMods.empty() || GetItemCountInInventory(apItem) != 1)
        return;

    using TModifyInventoryItemMod = bool(void*, uint32_t, TESObjectREFR*, TESBoundObject*, TESForm*, bool);
    static VersionDbPtr<TModifyInventoryItemMod> modifyInventoryItemMod(2254249);

    ScopedInventoryOverride inventoryOverride;
    ScopedEquipOverride equipOverride;

    auto& modSystem = World::Get().GetModSystem();
    for (const auto& modId : acMods)
    {
        if (auto* pMod = TESForm::GetById(modSystem.GetGameId(modId)))
            modifyInventoryItemMod.Get()(nullptr, 0, this, apItem, pMod, true);
    }
}

void TESObjectREFR::PayGold(int32_t aAmount) noexcept
{
    ScopedInventoryOverride _;

    // Caps001
    Inventory::Entry caps{};
    World::Get().GetModSystem().GetServerModId(0xF, caps.BaseId);
    caps.Count = -aAmount;
    AddOrRemoveItem(caps);
}

namespace
{
TP_THIS_FUNCTION(TAddObjectToContainer, void, TESObjectREFR, TESBoundObject*, ExtraDataList**, int32_t, TESObjectREFR*, ITEM_REMOVE_REASON);
TP_THIS_FUNCTION(TRemoveItem, BSPointerHandle<TESObjectREFR>*, TESObjectREFR, BSPointerHandle<TESObjectREFR>*, RemoveItemData*);

TAddObjectToContainer* RealAddObjectToContainer = nullptr;
TRemoveItem* RealRemoveItem = nullptr;

// Actors have their own hooks; Actor::RemoveItem also ends up in the reference version.
void QueueContainerChange(TESObjectREFR* apReference, const TESBoundObject* apObject, int32_t aCount)
{
    if (aCount == 0 || apReference->formType == FormType::Character || ScopedInventoryOverride::IsOverriden())
        return;

    Inventory::Entry item{};
    World::Get().GetModSystem().GetServerModId(apObject->formID, item.BaseId);
    item.Count = aCount;
    World::Get().GetRunner().Trigger(InventoryChangeEvent(apReference->formID, std::move(item)));
}

bool TP_MAKE_THISCALL(HookActivateRef, TESObjectREFR, TESObjectREFR* apActivator, TESBoundObject* apObjectToGet, int32_t aCount, bool aDefaultProcessing,
                      bool aFromScript, bool aLooping)
{
    Actor* pActivator = Cast<Actor>(apActivator);
    if (pActivator && apThis->baseForm && apThis->baseForm->formType != FormType::Book && !ScopedActivateOverride::IsOverriden())
    {
        auto openState = TESObjectREFR::kNone;
        if (apThis->baseForm->formType == FormType::Door)
            openState = apThis->GetOpenState();

        World::Get().GetRunner().Trigger(ActivateEvent(apThis, pActivator, apObjectToGet, aCount, aDefaultProcessing, 0, openState));
    }

    return TiltedPhoques::ThisCall(RealActivateRef, apThis, apActivator, apObjectToGet, aCount, aDefaultProcessing, aFromScript, aLooping);
}

void TP_MAKE_THISCALL(HookAddLockChange, TESObjectREFR)
{
    TiltedPhoques::ThisCall(RealAddLockChange, apThis);

    if (const auto* pLock = apThis->GetLock())
        World::Get().GetRunner().Trigger(LockChangeEvent(apThis->formID, pLock->IsLocked(), pLock->lockLevel));
    else
        World::Get().GetRunner().Trigger(LockChangeEvent(apThis->formID, false, 0));
}

void TP_MAKE_THISCALL(HookAddObjectToContainer, TESObjectREFR, TESBoundObject* apObject, ExtraDataList** apExtra, int32_t aCount, TESObjectREFR* apOldContainer,
                      ITEM_REMOVE_REASON aReason)
{
    if (apObject && aCount > 0)
        QueueContainerChange(apThis, apObject, aCount);

    TiltedPhoques::ThisCall(RealAddObjectToContainer, apThis, apObject, apExtra, aCount, apOldContainer, aReason);
}

BSPointerHandle<TESObjectREFR>* TP_MAKE_THISCALL(HookRemoveItem, TESObjectREFR, BSPointerHandle<TESObjectREFR>* apResult, RemoveItemData* apData)
{
    // The engine asks for INT_MAX to mean "all of them".
    if (apData && apData->object && apData->count > 0)
        QueueContainerChange(apThis, apData->object, -static_cast<int32_t>(std::min<int64_t>(apData->count, apThis->GetItemCountInInventory(apData->object))));

    return TiltedPhoques::ThisCall(RealRemoveItem, apThis, apResult, apData);
}
} // namespace

static TiltedPhoques::Initializer s_referenceHooks(
    []()
    {
        static VersionDbPtr<TActivateRef> activateRef(2201147);
        static VersionDbPtr<TAddLockChange> addLockChange(2200731);
        static VersionDbPtr<TAddObjectToContainer> addObjectToContainer(2201031);
        static VersionDbPtr<TRemoveItem> removeItem(2200919);

        RealActivateRef = activateRef.Get();
        RealAddLockChange = addLockChange.Get();
        RealAddObjectToContainer = addObjectToContainer.Get();
        RealRemoveItem = removeItem.Get();

        TP_HOOK(&RealActivateRef, HookActivateRef);
        TP_HOOK(&RealAddLockChange, HookAddLockChange);
        TP_HOOK(&RealAddObjectToContainer, HookAddObjectToContainer);
        TP_HOOK(&RealRemoveItem, HookRemoveItem);
    });
