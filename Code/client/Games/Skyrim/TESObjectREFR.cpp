#include <TiltedOnlinePCH.h>

#include <Games/References.h>
#include <Games/Overrides.h>

#include <World.h>
#include <Services/PapyrusService.h>
#include <Events/ActivateEvent.h>
#include <Events/InventoryChangeEvent.h>
#include <Events/ScriptAnimationEvent.h>

#include <ExtraData/ExtraDataList.h>
#include <ExtraData/ExtraCharge.h>
#include <ExtraData/ExtraCount.h>
#include <ExtraData/ExtraEnchantment.h>
#include <ExtraData/ExtraHealth.h>
#include <ExtraData/ExtraPoison.h>
#include <ExtraData/ExtraSoul.h>
#include <ExtraData/ExtraTextDisplayData.h>
#include <ExtraData/ExtraWorn.h>
#include <ExtraData/ExtraWornLeft.h>
#include <Forms/EnchantmentItem.h>
#include <Forms/AlchemyItem.h>
#include <EquipManager.h>
#include <DefaultObjectManager.h>

TP_THIS_FUNCTION(TActivate, void, TESObjectREFR, TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* apObjectToGet, int32_t aCount, char aDefaultProcessing);
TP_THIS_FUNCTION(TAddInventoryItem, void, TESObjectREFR, TESBoundObject* apItem, ExtraDataList* apExtraData, int32_t aCount, TESObjectREFR* apOldOwner);
TP_THIS_FUNCTION(TRemoveInventoryItem, BSPointerHandle<TESObjectREFR>*, TESObjectREFR, BSPointerHandle<TESObjectREFR>* apResult, TESBoundObject* apItem, int32_t aCount, ITEM_REMOVE_REASON aReason, ExtraDataList* apExtraList, TESObjectREFR* apMoveToRef, const NiPoint3* apDropLoc, const NiPoint3* apRotate);
TP_THIS_FUNCTION(TPlayAnimationAndWait, bool, void, uint32_t auiStackID, TESObjectREFR* apSelf, BSFixedString* apAnimation, BSFixedString* apEventName);
TP_THIS_FUNCTION(TPlayAnimation, bool, void, uint32_t auiStackID, TESObjectREFR* apSelf, BSFixedString* apEventName);

static TActivate* RealActivate = nullptr;
static TAddInventoryItem* RealAddInventoryItem = nullptr;
static TRemoveInventoryItem* RealRemoveInventoryItem = nullptr;
static TPlayAnimationAndWait* RealPlayAnimationAndWait = nullptr;
static TPlayAnimation* RealPlayAnimation = nullptr;

#ifdef SAVE_STUFF

#include <Games/Skyrim/SaveLoad.h>

void TESObjectREFR::Save_Reversed(const uint32_t aChangeFlags, Buffer::Writer& aWriter)
{
    TESForm::Save_Reversed(aChangeFlags, aWriter);

    if(aChangeFlags & CHANGE_REFR_BASEOBJECT)
    {
        // save baseForm->formID;
        // we don't because each player has it's own form id system
    }

    if(aChangeFlags & CHANGE_REFR_SCALE)
    {
        // So skyrim does some weird conversion shit here, we are going to do the same for now
        float fScale = scale;
        fScale /= 100.f;
        aWriter.WriteBytes((uint8_t*)& fScale, 4);
    }

    // So skyrim 
    uint32_t extraFlags = 0xA6021C40;
    if(formType == Character)
        extraFlags = 0xA6061840;

    if(aChangeFlags & extraFlags)
    {
        // We have flags to save
    }

    if(aChangeFlags & (CHANGE_REFR_INVENTORY | CHANGE_REFR_LEVELED_INVENTORY))
    {
        
    }

    if (aChangeFlags & CHANGE_REFR_ANIMATION)
    {
        // do something with animations
        // get extradata 0x41
    }


}

#endif

ExtraContainerChanges::Data* TESObjectREFR::GetContainerChanges() const noexcept
{
    TP_THIS_FUNCTION(TGetContainterChanges, ExtraContainerChanges::Data*, const TESObjectREFR);

    POINTER_SKYRIMSE(TGetContainterChanges, s_getContainerChangs, 16040);
    
    return ThisCall(s_getContainerChangs, this);
}

void TESObjectREFR::RemoveAllItems() noexcept
{
    using ObjectReference = TESObjectREFR;

    PAPYRUS_FUNCTION(void, ObjectReference, RemoveAllItems, TESObjectREFR*, bool, bool);

    ScopedEquipOverride equipOverride;

    s_pRemoveAllItems(this, nullptr, false, true);
}

TESContainer* TESObjectREFR::GetContainer() const noexcept
{
    TP_THIS_FUNCTION(TGetContainer, TESContainer*, const TESObjectREFR);

    POINTER_SKYRIMSE(TGetContainer, s_getContainer, 19702);

    return ThisCall(s_getContainer, this);
}

int64_t TESObjectREFR::GetItemCountInInventory(TESForm* apItem) const noexcept
{
    int64_t count = GetContainer()->GetItemCount(apItem);

    auto* pContainerChanges = GetContainerChanges()->entries;
    for (auto pChange : *pContainerChanges)
    {
        if (pChange && pChange->form)
        {
            if (pChange->form->formID == apItem->formID)
            {
                count += pChange->count;
                break;
            }
        }
    }

    return count;
}

TESObjectCELL* TESObjectREFR::GetParentCellEx() const noexcept
{
    return parentCell ? parentCell : GetParentCell();
}

void TESObjectREFR::GetItemFromExtraData(Inventory::Entry& arEntry, ExtraDataList* apExtraDataList) noexcept
{
    auto& modSystem = World::Get().GetModSystem();

    if (ExtraCount* pExtraCount = Cast<ExtraCount>(apExtraDataList->GetByType(ExtraDataType::Count)))
    {
        arEntry.Count = pExtraCount->count;
    }

    if (ExtraCharge* pExtraCharge = Cast<ExtraCharge>(apExtraDataList->GetByType(ExtraDataType::Charge)))
    {
        arEntry.ExtraCharge = pExtraCharge->fCharge;
    }

    if (ExtraEnchantment* pExtraEnchantment = Cast<ExtraEnchantment>(apExtraDataList->GetByType(ExtraDataType::Enchantment)))
    {
        TP_ASSERT(pExtraEnchantment->pEnchantment, "Null enchantment in ExtraEnchantment");

        modSystem.GetServerModId(pExtraEnchantment->pEnchantment->formID, arEntry.ExtraEnchantId);

        if (pExtraEnchantment->pEnchantment->formID & 0xFF000000)
        {
            for (EffectItem* pEffectItem : pExtraEnchantment->pEnchantment->listOfEffects)
            {
                TP_ASSERT(pEffectItem, "pEffectItem is null.");
                if (!pEffectItem)
                    continue;

                Inventory::EffectItem effect;
                effect.Magnitude = pEffectItem->data.fMagnitude;
                effect.Area = pEffectItem->data.iArea;
                effect.Duration = pEffectItem->data.iDuration;
                effect.RawCost = pEffectItem->fRawCost;
                modSystem.GetServerModId(pEffectItem->pEffectSetting->formID, effect.EffectId);
                arEntry.EnchantData.Effects.push_back(effect);
            }

            uint32_t objectId = modSystem.GetGameId(arEntry.BaseId);
            TESForm* pObject = TESForm::GetById(objectId);
            if (pObject)
                arEntry.EnchantData.IsWeapon = pObject->formType == FormType::Weapon;
        }

        arEntry.ExtraEnchantCharge = pExtraEnchantment->usCharge;
        arEntry.ExtraEnchantRemoveUnequip = pExtraEnchantment->bRemoveOnUnequip;
    }

    if (ExtraHealth* pExtraHealth = Cast<ExtraHealth>(apExtraDataList->GetByType(ExtraDataType::Health)))
    {
        arEntry.ExtraHealth = pExtraHealth->fHealth;
    }

    if (ExtraPoison* pExtraPoison = Cast<ExtraPoison>(apExtraDataList->GetByType(ExtraDataType::Poison)))
    {
        TP_ASSERT(pExtraPoison->pPoison, "Null poison in ExtraPoison");
        if (pExtraPoison && pExtraPoison->pPoison)
        {
            modSystem.GetServerModId(pExtraPoison->pPoison->formID, arEntry.ExtraPoisonId);
            arEntry.ExtraPoisonCount = pExtraPoison->uiCount;
        }
    }

    if (ExtraSoul* pExtraSoul = Cast<ExtraSoul>(apExtraDataList->GetByType(ExtraDataType::Soul)))
    {
        arEntry.ExtraSoulLevel = (int32_t)pExtraSoul->cSoul;
    }

    /*
    if (ExtraTextDisplayData* pExtraTextDisplayData = Cast<ExtraTextDisplayData>(apExtraDataList->GetByType(ExtraDataType::TextDisplayData)))
    {
        if (pExtraTextDisplayData->DisplayName)
            arEntry.ExtraTextDisplayName = pExtraTextDisplayData->DisplayName;
        else
            arEntry.ExtraTextDisplayName = "";
    }
    */

    arEntry.ExtraWorn = apExtraDataList->Contains(ExtraDataType::Worn);
    arEntry.ExtraWornLeft = apExtraDataList->Contains(ExtraDataType::WornLeft);

    arEntry.IsQuestItem = apExtraDataList->HasQuestObjectAlias();
}

ExtraDataList* TESObjectREFR::GetExtraDataFromItem(const Inventory::Entry& arEntry) noexcept
{
    auto& modSystem = World::Get().GetModSystem();

    ExtraDataList* pExtraDataList = nullptr;

    if (!arEntry.ContainsExtraData())
        return pExtraDataList;

    pExtraDataList = ExtraDataList::New();

    if (arEntry.ExtraCharge > 0.f)
    {
        pExtraDataList->SetChargeData(arEntry.ExtraCharge);
    }

    if (arEntry.ExtraEnchantId != 0)
    {
        EnchantmentItem* pEnchantment = nullptr;
        if (arEntry.ExtraEnchantId.ModId == 0xFFFFFFFF)
        {
            pEnchantment = EnchantmentItem::Create(arEntry.EnchantData);
        }
        else
        {
            uint32_t enchantId = modSystem.GetGameId(arEntry.ExtraEnchantId);
            pEnchantment = Cast<EnchantmentItem>(TESForm::GetById(enchantId));
        }

        TP_ASSERT(pEnchantment, "No Enchantment created or found.");

        pExtraDataList->SetEnchantmentData(pEnchantment, arEntry.ExtraEnchantCharge,
                                           arEntry.ExtraEnchantRemoveUnequip);
    }

    if (arEntry.ExtraPoisonId != 0)
    {
        // TODO: does poison have the same temp problem as enchants?
        // doesn't seem to be the case, there are only like 3 poisons, and no custom ones
        TP_ASSERT(arEntry.ExtraPoisonId.ModId != 0xFFFFFFFF, "Poison is sent as temp!");

        uint32_t poisonId = modSystem.GetGameId(arEntry.ExtraPoisonId);
        if (AlchemyItem* pPoison = Cast<AlchemyItem>(TESForm::GetById(poisonId)))
        {
            pExtraDataList->SetPoison(pPoison, arEntry.ExtraPoisonCount);
        }
    }

    if (arEntry.ExtraHealth > 0.f)
    {
        pExtraDataList->SetHealth(arEntry.ExtraHealth);
    }

    if (arEntry.ExtraSoulLevel > 0 && arEntry.ExtraSoulLevel <= 5)
    {
        pExtraDataList->SetSoulData(static_cast<SOUL_LEVEL>(arEntry.ExtraSoulLevel));
    }

    if (arEntry.ExtraWorn)
    {
        pExtraDataList->SetWorn(false);
    }

    if (arEntry.ExtraWornLeft)
    {
        pExtraDataList->SetWorn(true);
    }

    // TODO: this is causing crashes
    /*
    if (!arEntry.ExtraTextDisplayName.empty())
    {
        ExtraTextDisplayData* pExtraText = Memory::Allocate<ExtraTextDisplayData>();
        *((uint64_t*)pExtraText) = 0x1416244D0;
        pExtraText->next = nullptr;
        pExtraText->DisplayName = arEntry.ExtraTextDisplayName.c_str();
        pExtraText->usCustomNameLength = arEntry.ExtraTextDisplayName.length();
        pExtraText->iOwnerInstance = -2;
        pExtraText->fTemperFactor = 1.0F;
        pExtraDataList->Add(ExtraDataType::TextDisplayData, pExtraText);
    }
    */

    if (pExtraDataList->data == nullptr)
    {
        Memory::Delete(pExtraDataList->bitfield);
        Memory::Delete(pExtraDataList);
        pExtraDataList = nullptr;
    }

    return pExtraDataList;
}

Inventory TESObjectREFR::GetInventory() const noexcept
{
    return GetInventory([](TESForm& aForm) { return true; });
}

Inventory TESObjectREFR::GetInventory(std::function<bool(TESForm&)> aFilter) const noexcept
{
    auto& modSystem = World::Get().GetModSystem();
    Inventory inventory{};

    if (TESContainer* pBaseContainer = GetContainer())
    {
        for (int i = 0; i < pBaseContainer->count; i++)
        {
            TESContainer::Entry* pGameEntry = pBaseContainer->entries[i];
            if (!pGameEntry || !pGameEntry->form)
            {
                spdlog::warn("Entry or form for inventory item is null.");
                continue;
            }

            if (!aFilter(*pGameEntry->form))
                continue;

            Inventory::Entry entry;
            modSystem.GetServerModId(pGameEntry->form->formID, entry.BaseId);
            entry.Count = pGameEntry->count;

            inventory.Entries.push_back(std::move(entry));
        }
    }

    Inventory extraInventory{};

    auto pExtraContChangesEntries = GetContainerChanges()->entries;
    for (auto pGameEntry : *pExtraContChangesEntries)
    {
        if (!pGameEntry)
            continue;

        if (!aFilter(*pGameEntry->form))
            continue;

        Inventory::Entry entry{};
        modSystem.GetServerModId(pGameEntry->form->formID, entry.BaseId);
        entry.Count = pGameEntry->count;

        for (ExtraDataList* pExtraDataList : *pGameEntry->dataList)
        {
            if (!pExtraDataList)
                continue;

            Inventory::Entry innerEntry;
            innerEntry.BaseId = entry.BaseId;
            innerEntry.Count = 1;

            GetItemFromExtraData(innerEntry, pExtraDataList);

            entry.Count -= innerEntry.Count;

            extraInventory.Entries.push_back(std::move(innerEntry));
        }

        if (entry.Count != 0)
            extraInventory.Entries.push_back(std::move(entry));
    }

    spdlog::debug("ExtraInventory count: {}", extraInventory.Entries.size());

    Inventory minimizedExtraInventory{};

    for (auto& entry : extraInventory.Entries)
    {
        auto duplicate = std::find_if(minimizedExtraInventory.Entries.begin(), minimizedExtraInventory.Entries.end(), [entry](const Inventory::Entry& newEntry) { 
            return newEntry.CanBeMerged(entry);
        });

        if (duplicate == std::end(minimizedExtraInventory.Entries))
        {
            minimizedExtraInventory.Entries.push_back(entry);
            continue;
        }

        duplicate->Count += entry.Count;
    }

    spdlog::debug("MinExtraInventory count: {}", minimizedExtraInventory.Entries.size());

    for (auto& entry : minimizedExtraInventory.Entries)
    {
        if (entry.ContainsExtraData())
            continue;

        auto duplicate = std::find_if(inventory.Entries.begin(), inventory.Entries.end(), [entry](const Inventory::Entry& newEntry) { 
            return newEntry.CanBeMerged(entry);
        });

        if (duplicate == std::end(inventory.Entries))
            continue;

        entry.Count += duplicate->Count;
        duplicate->Count = 0;
    }

    spdlog::debug("MinExtraInventory count after: {}", minimizedExtraInventory.Entries.size());

    inventory.Entries.insert(inventory.Entries.end(), minimizedExtraInventory.Entries.begin(),
                                 minimizedExtraInventory.Entries.end());

    spdlog::debug("Inventory count before: {}", inventory.Entries.size());

    inventory.RemoveByFilter([](const auto& entry) { return entry.Count == 0; });

    spdlog::debug("Inventory count after: {}", inventory.Entries.size());

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

bool TESObjectREFR::IsItemInInventory(uint32_t aFormID) const noexcept
{
    Inventory inventory = GetInventory([aFormID](TESForm& aForm) { return aForm.formID == aFormID; });
    return !inventory.Entries.empty();
}

void TESObjectREFR::SetInventory(const Inventory& aInventory) noexcept
{
    spdlog::debug("Setting inventory for {:X}", formID);

    ScopedInventoryOverride _;

    RemoveAllItems();

    for (const Inventory::Entry& entry : aInventory.Entries)
    {
        if (entry.Count != 0)
            AddOrRemoveItem(entry, true);
    }
}

void TESObjectREFR::AddOrRemoveItem(const Inventory::Entry& arEntry, bool aIsSettingInventory) noexcept
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

    ExtraDataList* pExtraDataList = GetExtraDataFromItem(arEntry);

    if (arEntry.Count > 0)
    {
        bool isWorn = false;
        bool isWornLeft = false;
        if (pExtraDataList)
        {
            isWorn = pExtraDataList->Contains(ExtraDataType::Worn);
            isWornLeft = pExtraDataList->Contains(ExtraDataType::WornLeft);
        }

        spdlog::debug("Adding item {:X}, count {}", pObject->formID, arEntry.Count);
        AddObjectToContainer(pObject, pExtraDataList, arEntry.Count, nullptr);

        // TODO: check Actor cast first?
        if (isWorn)
            EquipManager::Get()->Equip(Cast<Actor>(this), pObject, nullptr, arEntry.Count, DefaultObjectManager::Get().rightEquipSlot, false, true, false, false);
        else if (isWornLeft)
            EquipManager::Get()->Equip(Cast<Actor>(this), pObject, nullptr, arEntry.Count, DefaultObjectManager::Get().leftEquipSlot, false, true, false, false);
    }
    else if (arEntry.Count < 0)
    {
        spdlog::debug("Removing item {:X}, count {}", pObject->formID, -arEntry.Count);
        RemoveItem(pObject, -arEntry.Count, ITEM_REMOVE_REASON::kRemove, pExtraDataList, nullptr);
    }

    // TODO(cosideci): this is still flawed. Adding the refr to the quest leader is hard.
    // It is still recommended that the quest leader loots all quest items.
    if (arEntry.IsQuestItem && arEntry.Count > 0 && !aIsSettingInventory)
    {
        PlayerCharacter* pPlayer = PlayerCharacter::Get();

        if (!pPlayer->IsItemInInventory(objectId))
        {
            Actor* pActor = Cast<Actor>(this);
            if (pActor && pActor->GetExtension()->IsRemotePlayer())
                pPlayer->AddOrRemoveItem(arEntry);
        }
    }

    UpdateItemList(nullptr);
}

void TESObjectREFR::UpdateItemList(TESForm* pUnkForm) noexcept
{
    TP_THIS_FUNCTION(TUpdateItemList, void, TESObjectREFR, TESForm*);
    POINTER_SKYRIMSE(TUpdateItemList, updateItemList, 52849);
    ThisCall(updateItemList, this, pUnkForm);
}

void TESObjectREFR::Activate(TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* aObjectToGet, int32_t aCount, char aDefaultProcessing) noexcept
{
    ScopedActivateOverride _;

    return ThisCall(RealActivate, this, apActivator, aUnk1, aObjectToGet, aCount, aDefaultProcessing);
}

void TESObjectREFR::EnableImpl() noexcept
{
    TP_THIS_FUNCTION(TEnableImpl, void, TESObjectREFR, bool aResetInventory);

    POINTER_SKYRIMSE(TEnableImpl, s_enable, 19800);

    ThisCall(s_enable, this, false);
}

static thread_local bool s_cancelAnimationWaitEvent = false;

bool TESObjectREFR::PlayAnimationAndWait(BSFixedString* apAnimation, BSFixedString* apEventName) noexcept
{
    using ObjectReference = TESObjectREFR;

    PAPYRUS_FUNCTION(bool, ObjectReference, PlayAnimationAndWait, BSFixedString*, BSFixedString*);

    s_cancelAnimationWaitEvent = true;
    bool result = s_pPlayAnimationAndWait(this, apAnimation, apEventName);
    s_cancelAnimationWaitEvent = false;
    return result;
}

#define OBJECT_ANIM_SYNC 0

bool TP_MAKE_THISCALL(HookPlayAnimationAndWait, void, uint32_t auiStackID, TESObjectREFR* apSelf, BSFixedString* apAnimation, BSFixedString* apEventName)
{
    spdlog::debug("Animation: {}, EventName: {}", apAnimation->AsAscii(), apEventName->AsAscii());

#if OBJECT_ANIM_SYNC
    if (!s_cancelAnimationWaitEvent && (apSelf->formID < 0xFF000000))
        World::Get().GetRunner().Trigger(ScriptAnimationEvent(apSelf->formID, apAnimation->AsAscii(), apEventName->AsAscii()));
#endif

    return ThisCall(RealPlayAnimationAndWait, apThis, auiStackID, apSelf, apAnimation, apEventName);
}

static thread_local bool s_cancelAnimationEvent = false;

bool TESObjectREFR::PlayAnimation(BSFixedString* apEventName) noexcept
{
    using ObjectReference = TESObjectREFR;

    PAPYRUS_FUNCTION(bool, ObjectReference, PlayAnimation, BSFixedString*);

    s_cancelAnimationEvent = true;
    bool result = s_pPlayAnimation(this, apEventName);
    s_cancelAnimationEvent = false;
    return result;
}

bool TP_MAKE_THISCALL(HookPlayAnimation, void, uint32_t auiStackID, TESObjectREFR* apSelf, BSFixedString* apEventName)
{
    spdlog::debug("EventName: {}", apEventName->AsAscii());

#if OBJECT_ANIM_SYNC
    if (!s_cancelAnimationEvent && (apSelf->formID < 0xFF000000))
        World::Get().GetRunner().Trigger(ScriptAnimationEvent(apSelf->formID, String{}, apEventName->AsAscii()));
#endif

    return ThisCall(RealPlayAnimation, apThis, auiStackID, apSelf, apEventName);
}

void TP_MAKE_THISCALL(HookActivate, TESObjectREFR, TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* apObjectToGet, int32_t aCount, char aDefaultProcessing)
{
    Actor* pActivator = Cast<Actor>(apActivator);
    if (pActivator)
        World::Get().GetRunner().Trigger(ActivateEvent(apThis, pActivator, apObjectToGet, aUnk1, aCount, aDefaultProcessing));

    return ThisCall(RealActivate, apThis, apActivator, aUnk1, apObjectToGet, aCount, aDefaultProcessing);
}

void TP_MAKE_THISCALL(HookAddInventoryItem, TESObjectREFR, TESBoundObject* apItem, ExtraDataList* apExtraData, int32_t aCount, TESObjectREFR* apOldOwner)
{
    if (!ScopedInventoryOverride::IsOverriden())
    {
        auto& modSystem = World::Get().GetModSystem();

        Inventory::Entry item{};
        modSystem.GetServerModId(apItem->formID, item.BaseId);
        item.Count = aCount;

        if (apExtraData)
            apThis->GetItemFromExtraData(item, apExtraData);

        World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID, std::move(item)));
    }

    spdlog::debug("Adding inventory item {:X} to {:X}", apItem->formID, apThis->formID);

    ThisCall(RealAddInventoryItem, apThis, apItem, apExtraData, aCount, apOldOwner);
}

BSPointerHandle<TESObjectREFR>* TP_MAKE_THISCALL(HookRemoveInventoryItem, TESObjectREFR, BSPointerHandle<TESObjectREFR>* apResult, TESBoundObject* apItem, int32_t aCount, ITEM_REMOVE_REASON aReason, ExtraDataList* apExtraList, TESObjectREFR* apMoveToRef, const NiPoint3* apDropLoc, const NiPoint3* apRotate)
{
    if (!ScopedInventoryOverride::IsOverriden())
    {
        auto& modSystem = World::Get().GetModSystem();

        Inventory::Entry item{};
        modSystem.GetServerModId(apItem->formID, item.BaseId);

        if (apExtraList)
        {
            ScopedExtraDataOverride _;
            apThis->GetItemFromExtraData(item, apExtraList);
        }

        item.Count = -aCount;

        World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID, std::move(item)));
    }

    spdlog::debug("Removing inventory item {:X} from {:X}", apItem->formID, apThis->formID);

    ScopedEquipOverride _;

    return ThisCall(RealRemoveInventoryItem, apThis, apResult, apItem, aCount, aReason, apExtraList, apMoveToRef, apDropLoc, apRotate);
}

static TiltedPhoques::Initializer s_objectReferencesHooks([]() {
    POINTER_SKYRIMSE(TActivate, s_activate, 19796);
    POINTER_SKYRIMSE(TAddInventoryItem, s_addInventoryItem, 19708);
    POINTER_SKYRIMSE(TRemoveInventoryItem, s_removeInventoryItem, 19689);
    POINTER_SKYRIMSE(TPlayAnimationAndWait, s_playAnimationAndWait, 56206);
    POINTER_SKYRIMSE(TPlayAnimation, s_playAnimation, 56205);

    RealActivate = s_activate.Get();
    RealAddInventoryItem = s_addInventoryItem.Get();
    RealRemoveInventoryItem = s_removeInventoryItem.Get();
    RealPlayAnimationAndWait = s_playAnimationAndWait.Get();
    RealPlayAnimation = s_playAnimation.Get();

    TP_HOOK(&RealActivate, HookActivate);
    TP_HOOK(&RealAddInventoryItem, HookAddInventoryItem);
    TP_HOOK(&RealRemoveInventoryItem, HookRemoveInventoryItem);
    TP_HOOK(&RealPlayAnimationAndWait, HookPlayAnimationAndWait);
    TP_HOOK(&RealPlayAnimation, HookPlayAnimation);
});
