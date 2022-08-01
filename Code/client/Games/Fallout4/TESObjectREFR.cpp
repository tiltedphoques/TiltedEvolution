#include <TiltedOnlinePCH.h>
#include <Games/References.h>
#include <World.h>

#include <Events/ActivateEvent.h>
#include <Events/InventoryChangeEvent.h>

#include <Forms/TESBoundObject.h>
#include <Forms/TESObjectARMO.h>
#include <Forms/TESObjectWEAP.h>
#include <Forms/BGSObjectInstance.h>

#include <EquipManager.h>

#include <Games/Overrides.h>

TP_THIS_FUNCTION(TActivate, void, TESObjectREFR, TESObjectREFR* apActivator, TESBoundObject* apObjectToGet, int32_t aCount, bool aDefaultProcessing, bool aFromScript, bool aIsLooping);
TP_THIS_FUNCTION(TAddInventoryItem, void, TESObjectREFR, TESBoundObject* apObject, ExtraDataList* apExtraData, uint32_t aCount,
                 TESObjectREFR* apOldContainer, void* apUnk1, void* apUnk2);
TP_THIS_FUNCTION(TRemoveInventoryItem, uint32_t*, TESObjectREFR, uint32_t* apUnk1, void* apUnk2);

static TActivate* RealActivate = nullptr;
static TAddInventoryItem* RealAddInventoryItem = nullptr;
static TRemoveInventoryItem* RealRemoveInventoryItem = nullptr;

Inventory TESObjectREFR::GetInventory() const noexcept
{
    // TODO: inventory list has RWLock, should probably use?

    Inventory inventory{};
    if (!pInventoryList)
        return inventory;

    auto& modSystem = World::Get().GetModSystem();

    for (auto& item : pInventoryList->DataA)
    {
        if (!item.pObject || !item.spStackData)
            continue;

        GameId baseId{};
        modSystem.GetServerModId(item.pObject->formID, baseId);

        for (auto* pStack = item.spStackData; pStack; pStack = pStack->spNextStack)
        {
            Inventory::Entry entry{};
            entry.BaseId = baseId;
            entry.Count = pStack->uiCount;

            // If an item with multiple copies is equipped, they will be in the same stack.
            // The stacks only make distinctions between modified items.
            if (pStack->usFlags != BGSInventoryItem::Stack::Flag::INV_SLOT_INDEX_1)
                entry.ExtraWorn = true;

            inventory.Entries.push_back(std::move(entry));
        }
    }

    return inventory;
}

void TESObjectREFR::SetInventory(const Inventory& aInventory) noexcept
{
    ScopedInventoryOverride _;

    RemoveAllItems();

    for (const Inventory::Entry& entry : aInventory.Entries)
    {
        if (entry.Count != 0)
            AddOrRemoveItem(entry);
    }
}

void TESObjectREFR::RemoveAllItems() noexcept
{
    using TRemoveAllItems = void(void*, void*, TESObjectREFR*, TESObjectREFR*, bool);
    POINTER_FALLOUT4(TRemoveAllItems, s_removeAllItems, 534059);
    s_removeAllItems(nullptr, nullptr, this, nullptr, false);
}

void TESObjectREFR::AddOrRemoveItem(const Inventory::Entry& arEntry) noexcept
{
    ModSystem& modSystem = World::Get().GetModSystem();

    uint32_t objectId = modSystem.GetGameId(arEntry.BaseId);
    TESBoundObject* pObject = Cast<TESBoundObject>(TESForm::GetById(objectId));
    if (!pObject)
    {
        spdlog::warn("{}: Object to add not found, {:X}:{:X}.", __FUNCTION__, arEntry.BaseId.ModId,
                     arEntry.BaseId.BaseId);
        return;
    }

    if (arEntry.Count > 0)
    {
        ExtraDataList list{};
        AddObjectToContainer(pObject, &list, arEntry.Count, nullptr);

        if (arEntry.IsWorn())
        {
            if (Actor* pActor = Cast<Actor>(this))
            {
                BGSObjectInstance object(pObject, nullptr);
                EquipManager::Get()->EquipObject(pActor, object, 0, arEntry.Count, nullptr, false, true, false, false, false);
            }
        }
    }
    else if (arEntry.Count < 0)
    {
        RemoveItemData data{};
        data.pObject = pObject;
        data.iNumber = arEntry.Count;
        // TODO(cosideci): is NONE valid?
        data.eReason = ITEM_REMOVE_REASON::IRR_NONE;
        data.pOtherContainer = nullptr;
        data.pDropLoc = nullptr;
        data.pRotate = nullptr;

        RemoveItem(&data);
    }
}

ActorValueInfo* TESObjectREFR::GetActorValueInfo(uint32_t aId) const noexcept
{
    using TGetActorValueInfoArray = ActorValueInfo**();

    POINTER_FALLOUT4(TGetActorValueInfoArray, s_getActorValueInfoArray, 405391);

    ActorValueInfo** actorValueInfoArray = s_getActorValueInfoArray.Get()();

    return actorValueInfoArray[aId];
}

const BGSEquipSlot* TESObjectREFR::GetEquipSlot(uint32_t aEquipIndex) const noexcept
{
    TP_THIS_FUNCTION(TGetEquipSlot, const BGSEquipSlot*, const TESObjectREFR, uint32_t);

    POINTER_FALLOUT4(TGetEquipSlot, getEquipSlot, 180924);

    return ThisCall(getEquipSlot, this, aEquipIndex);
}

void TESObjectREFR::Activate(TESObjectREFR* apActivator, TESBoundObject* apObjectToGet, int32_t aCount, bool aDefaultProcessing, bool aFromScript, bool aIsLooping) noexcept
{
    return ThisCall(RealActivate, this, apActivator, apObjectToGet, aCount, aDefaultProcessing, aFromScript, aIsLooping);
}

void TP_MAKE_THISCALL(HookActivate, TESObjectREFR, TESObjectREFR* apActivator, TESBoundObject* apObjectToGet, int aCount, bool aDefaultProcessing, bool aFromScript, bool aIsLooping)
{
    auto* pActivator = Cast<Actor>(apActivator);
    if (pActivator)
        World::Get().GetRunner().Trigger(ActivateEvent(apThis, pActivator, apObjectToGet, aCount, aDefaultProcessing, aFromScript, aIsLooping));

    return ThisCall(RealActivate, apThis, apActivator, apObjectToGet, aCount, aDefaultProcessing, aFromScript, aIsLooping);
}

void TP_MAKE_THISCALL(HookAddInventoryItem, TESObjectREFR, TESBoundObject* apObject, ExtraDataList* apExtraData,
                      uint32_t aCount, TESObjectREFR* apOldContainer, void* apUnk1, void* apUnk2)
{
    // TODO: ft
#if TP_SKYRIM64
    World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID));
#endif
    ThisCall(RealAddInventoryItem, apThis, apObject, apExtraData, aCount, apOldContainer, apUnk1, apUnk2);
}

uint32_t* TP_MAKE_THISCALL(HookRemoveInventoryItem, TESObjectREFR, uint32_t* apUnk1, void* apUnk2)
{
    // TODO: ft
#if TP_SKYRIM64
    World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID));
#endif
    return ThisCall(RealRemoveInventoryItem, apThis, apUnk1, apUnk2);
}

static TiltedPhoques::Initializer s_objectReferencesHooks([]() {
        POINTER_FALLOUT4(TActivate, s_activate, 753532);
        POINTER_FALLOUT4(TAddInventoryItem, s_addItem, 78186);
        POINTER_FALLOUT4(TRemoveInventoryItem, s_removeItem, 943234);

        RealActivate = s_activate.Get();
        RealAddInventoryItem = s_addItem.Get();
        RealRemoveInventoryItem = s_removeItem.Get();

        TP_HOOK(&RealActivate, HookActivate);
        TP_HOOK(&RealAddInventoryItem, HookAddInventoryItem);
        TP_HOOK(&RealRemoveInventoryItem, HookRemoveInventoryItem);
});
