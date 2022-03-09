#include <TiltedOnlinePCH.h>

#include <Games/References.h>
#include <Games/Overrides.h>

#include <World.h>
#include <Services/PapyrusService.h>
#include <Events/ActivateEvent.h>
#include <Events/InventoryChangeEvent.h>
#include <Events/ScriptAnimationEvent.h>

#include <Games/ExtraDataList.h>
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
#include <EquipManager.h>

TP_THIS_FUNCTION(TActivate, void, TESObjectREFR, TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* apObjectToGet, int32_t aCount, char aDefaultProcessing);
TP_THIS_FUNCTION(TAddInventoryItem, void*, TESObjectREFR, TESBoundObject* apItem, ExtraDataList* apExtraData, uint32_t aCount, TESObjectREFR* apOldOwner);
TP_THIS_FUNCTION(TRemoveInventoryItem, void*, TESObjectREFR, float* apUnk0, TESBoundObject* apItem, uint32_t aCount, uint32_t aUnk1, ExtraDataList* apExtraData, TESObjectREFR* apNewOwner, NiPoint3* apUnk2, NiPoint3* apUnk3);
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

void TESObjectREFR::SaveInventory(BGSSaveFormBuffer* apBuffer) const noexcept
{
    auto changes = GetContainerChanges();

    auto entries = changes->entries;
    uint32_t entryCount = 0;
    for (auto entry : *entries)
    {
        entryCount++;
    }
    if (entryCount > 1024)
        spdlog::error("Inventory entry count is really big: {:X}:{}", formID, entryCount);

    changes->Save(apBuffer);
}

void TESObjectREFR::LoadInventory(BGSLoadFormBuffer* apBuffer) noexcept
{
    GetContainerChanges()->Load(apBuffer);
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

Inventory TESObjectREFR::GetInventory() const noexcept
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

        Inventory::Entry entry;
        modSystem.GetServerModId(pGameEntry->form->formID, entry.BaseId);
        entry.Count = pGameEntry->count;

        for (ExtraDataList* pExtraDataList : *pGameEntry->dataList)
        {
            if (!pExtraDataList)
                continue;

            Inventory::Entry innerEntry;
            innerEntry.BaseId = entry.BaseId;
            innerEntry.Count = 1;

            if (ExtraCount* pExtraCount = (ExtraCount*)pExtraDataList->GetByType(ExtraData::Count))
            {
                innerEntry.Count = pExtraCount->count;
            }

            if (ExtraCharge* pExtraCharge = (ExtraCharge*)pExtraDataList->GetByType(ExtraData::Charge))
            {
                innerEntry.ExtraCharge = pExtraCharge->fCharge;
            }

            if (ExtraEnchantment* pExtraEnchantment = (ExtraEnchantment*)pExtraDataList->GetByType(ExtraData::Enchantment))
            {
                TP_ASSERT(pExtraEnchantment->pEnchantment, "Null enchantment in ExtraEnchantment");

                modSystem.GetServerModId(pExtraEnchantment->pEnchantment->formID, innerEntry.ExtraEnchantId);

                if (pExtraEnchantment->pEnchantment->formID & 0xFF000000)
                {
                    for (EffectItem* pEffectItem : pExtraEnchantment->pEnchantment->listOfEffects)
                    {
                        TP_ASSERT(pEffectItem, "pEffectItem is null.");

                        Inventory::EffectItem effect;
                        effect.Magnitude = pEffectItem->data.fMagnitude;
                        effect.Area = pEffectItem->data.iArea;
                        effect.Duration = pEffectItem->data.iDuration;
                        effect.RawCost = pEffectItem->fRawCost;
                        modSystem.GetServerModId(pEffectItem->pEffectSetting->formID, effect.EffectId);
                        innerEntry.EnchantData.Effects.push_back(effect);
                    }

                    uint32_t objectId = modSystem.GetGameId(innerEntry.BaseId);
                    innerEntry.EnchantData.IsWeapon = TESForm::GetById(objectId)->formType == FormType::Weapon;
                }

                innerEntry.ExtraEnchantCharge = pExtraEnchantment->usCharge;
                innerEntry.ExtraEnchantRemoveUnequip = pExtraEnchantment->bRemoveOnUnequip;
            }

            if (ExtraHealth* pExtraHealth = (ExtraHealth*)pExtraDataList->GetByType(ExtraData::Health))
            {
                innerEntry.ExtraHealth = pExtraHealth->fHealth;
            }

            if (ExtraPoison* pExtraPoison = (ExtraPoison*)pExtraDataList->GetByType(ExtraData::Poison))
            {
                TP_ASSERT(pExtraPoison->pPoison, "Null poison in ExtraPoison");
                modSystem.GetServerModId(pExtraPoison->pPoison->formID, innerEntry.ExtraPoisonId);
                innerEntry.ExtraPoisonCount = pExtraPoison->uiCount;
            }

            if (ExtraSoul* pExtraSoul = (ExtraSoul*)pExtraDataList->GetByType(ExtraData::Soul))
            {
                innerEntry.ExtraSoulLevel = (int32_t)pExtraSoul->cSoul;
            }

            if (ExtraTextDisplayData* pExtraTextDisplayData = (ExtraTextDisplayData*)pExtraDataList->GetByType(ExtraData::TextDisplayData))
            {
                if (pExtraTextDisplayData->DisplayName)
                    innerEntry.ExtraTextDisplayName = pExtraTextDisplayData->DisplayName;
                else
                    innerEntry.ExtraTextDisplayName = "";
            }

            innerEntry.ExtraWorn = pExtraDataList->Contains(ExtraData::Worn);
            innerEntry.ExtraWornLeft = pExtraDataList->Contains(ExtraData::WornLeft);

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

    inventory.Entries.erase(std::remove_if(inventory.Entries.begin(), inventory.Entries.end(),
                                           [](const Inventory::Entry& entry) { return entry.Count == 0; }),
                            inventory.Entries.end());

    spdlog::debug("Inventory count after: {}", inventory.Entries.size());

    return inventory;
}

thread_local bool g_modifyingInventory = false;

void TESObjectREFR::SetInventory(Inventory& aInventory) noexcept
{
    g_modifyingInventory = true;

    RemoveAllItems();

    for (const Inventory::Entry& entry : aInventory.Entries)
    {
        if (entry.Count != 0)
            AddItem(entry);
    }

    g_modifyingInventory = false;
}

void TESObjectREFR::AddItem(const Inventory::Entry& arEntry) noexcept
{
    ModSystem& modSystem = World::Get().GetModSystem();

    uint32_t objectId = modSystem.GetGameId(arEntry.BaseId);
    TESBoundObject* pObject = RTTI_CAST(TESForm::GetById(objectId), TESForm, TESBoundObject);
    if (!pObject)
    {
        spdlog::warn("{}: Object to add not found, {:X}:{:X}.", __FUNCTION__, arEntry.BaseId.ModId, arEntry.BaseId.BaseId);
        return;
    }

    ExtraDataList* pExtraDataList = nullptr;

    if (arEntry.ContainsExtraData())
    {
        pExtraDataList = Memory::Allocate<ExtraDataList>();
        pExtraDataList->data = nullptr;
        pExtraDataList->lock.m_counter = pExtraDataList->lock.m_tid = 0;
        pExtraDataList->bitfield = Memory::Allocate<ExtraDataList::Bitfield>();
        memset(pExtraDataList->bitfield, 0, 0x18);

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
                pEnchantment = RTTI_CAST(TESForm::GetById(enchantId), TESForm, EnchantmentItem);
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
            if (AlchemyItem* pPoison = RTTI_CAST(TESForm::GetById(poisonId), TESForm, AlchemyItem))
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
            pExtraDataList->Add(ExtraData::TextDisplayData, pExtraText);
        }
        */

        if (pExtraDataList->data == nullptr)
        {
            Memory::Delete(pExtraDataList->bitfield);
            Memory::Delete(pExtraDataList);
            pExtraDataList = nullptr;
        }
    }

    AddObjectToContainer(pObject, pExtraDataList, arEntry.Count, nullptr);
    spdlog::debug("Added object to container, form id: {:X}, extra data count: {}, entry count: {}", pObject->formID,
                 pExtraDataList ? (int)pExtraDataList->GetCount() : -1, arEntry.Count);
}

void TESObjectREFR::Activate(TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* aObjectToGet, int32_t aCount, char aDefaultProcessing) noexcept
{
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
    auto* pActivator = RTTI_CAST(apActivator, TESObjectREFR, Actor);
    if (pActivator)
        World::Get().GetRunner().Trigger(ActivateEvent(apThis, pActivator, apObjectToGet, aUnk1, aCount, aDefaultProcessing));

    return ThisCall(RealActivate, apThis, apActivator, aUnk1, apObjectToGet, aCount, aDefaultProcessing);
}

void* TP_MAKE_THISCALL(HookAddInventoryItem, TESObjectREFR, TESBoundObject* apItem, ExtraDataList* apExtraData, uint32_t aCount, TESObjectREFR* apOldOwner)
{
    if (!g_modifyingInventory)
        World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID));
    return ThisCall(RealAddInventoryItem, apThis, apItem, apExtraData, aCount, apOldOwner);
}

void* TP_MAKE_THISCALL(HookRemoveInventoryItem, TESObjectREFR, float* apUnk0, TESBoundObject* apItem, uint32_t aCount, uint32_t aUnk1, ExtraDataList* apExtraData, TESObjectREFR* apNewOwner, NiPoint3* apUnk2, NiPoint3* apUnk3)
{
    if (!g_modifyingInventory)
        World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID));
    return ThisCall(RealRemoveInventoryItem, apThis, apUnk0, apItem, aCount, aUnk1, apExtraData, apNewOwner, apUnk2, apUnk3);
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
