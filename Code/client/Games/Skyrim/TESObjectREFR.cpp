#include <TiltedOnlinePCH.h>

#include <Games/References.h>
#include <Games/Overrides.h>

#include <World.h>
#include <Services/PapyrusService.h>
#include <Events/ActivateEvent.h>
#include <Events/InventoryChangeEvent.h>
#include <Events/ScriptAnimationEvent.h>

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

TP_THIS_FUNCTION(TActivate, void, TESObjectREFR, TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* apObjectToGet, int32_t aCount, char aDefaultProcessing);
TP_THIS_FUNCTION(TAddInventoryItem, void*, TESObjectREFR, TESBoundObject* apItem, BSExtraDataList* apExtraData, uint32_t aCount, TESObjectREFR* apOldOwner);
TP_THIS_FUNCTION(TRemoveInventoryItem, void*, TESObjectREFR, float* apUnk0, TESBoundObject* apItem, uint32_t aCount, uint32_t aUnk1, BSExtraDataList* apExtraData, TESObjectREFR* apNewOwner, NiPoint3* apUnk2, NiPoint3* apUnk3);
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

    POINTER_SKYRIMSE(TGetContainterChanges, s_getContainerChangs, 0x1401E47F0 - 0x140000000);
    
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

    POINTER_SKYRIMSE(TGetContainer, s_getContainer, 0x1402A05C0 - 0x140000000);

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

void TESObjectREFR::AddItem(const Container::Entry& arEntry) noexcept
{
    ModSystem& modSystem = World::Get().GetModSystem();

    uint32_t objectId = modSystem.GetGameId(arEntry.BaseId);
    TESBoundObject* pObject = RTTI_CAST(TESForm::GetById(objectId), TESForm, TESBoundObject);
    if (!pObject)
    {
        spdlog::warn("{}: Object to add not found, {:X}:{:X}.", __FUNCTION__, arEntry.BaseId.ModId, arEntry.BaseId.BaseId);
        return;
    }

    BSExtraDataList* pExtraData = nullptr;

    if (arEntry.ContainsExtraData())
    {
        pExtraData = Memory::Allocate<BSExtraDataList>();
        pExtraData->data = nullptr;
        pExtraData->lock.m_counter = pExtraData->lock.m_tid = 0;
        pExtraData->bitfield = Memory::Allocate<BSExtraDataList::Bitfield>();
        memset(pExtraData->bitfield, 0, 0x18);

        if (arEntry.ExtraCharge > 0.f)
        {
            ExtraCharge* pExtraCharge = Memory::Allocate<ExtraCharge>();
            *((uint64_t*)pExtraCharge) = 0x141623AB0;
            pExtraCharge->next = nullptr;
            pExtraCharge->fCharge = arEntry.ExtraCharge;
            pExtraData->Add(ExtraData::Charge, pExtraCharge);
        }

        // TODO: deal with temp forms for enchanted items
        if (arEntry.ExtraEnchantId != 0)
        {
            //TP_ASSERT(arEntry.ExtraEnchantId.ModId != 0xFFFFFFFF, "Enchantment is sent as temp!");

            if (arEntry.ExtraEnchantId.ModId != 0xFFFFFFFF)
            {
            
            EnchantmentItem* pEnchantment = nullptr;
            if (arEntry.ExtraEnchantId.ModId == 0xFFFFFFFF)
            {
                spdlog::warn("Creating EnchantmentItem for {:X}:{:X}", arEntry.BaseId.ModId, arEntry.BaseId.BaseId);
                pEnchantment = EnchantmentItem::Create(arEntry.EnchantData);
            }
            else
            {
                uint32_t enchantId = modSystem.GetGameId(arEntry.ExtraEnchantId);
                pEnchantment = RTTI_CAST(TESForm::GetById(enchantId), TESForm, EnchantmentItem);
            }

            ExtraEnchantment* pExtraEnchantment = Memory::Allocate<ExtraEnchantment>();
            *((uint64_t*)pExtraEnchantment) = 0x141623E70;
            pExtraEnchantment->next = nullptr;
            pExtraEnchantment->pEnchantment = pEnchantment;
            pExtraEnchantment->usCharge = arEntry.ExtraEnchantCharge;
            pExtraEnchantment->bRemoveOnUnequip = arEntry.ExtraEnchantRemoveUnequip;
            }
        }

        if (arEntry.ExtraHealth > 0.f)
        {
            ExtraHealth* pExtraHealth = Memory::Allocate<ExtraHealth>();
            *((uint64_t*)pExtraHealth) = 0x141623A50;
            pExtraHealth->next = nullptr;
            pExtraHealth->fHealth = arEntry.ExtraHealth;
            pExtraData->Add(ExtraData::Health, pExtraHealth);
        }

        // TODO: does poison have the same temp problem as enchants?
        // put an assert for now
        if (arEntry.ExtraPoisonId != 0)
        {
            TP_ASSERT(arEntry.ExtraPoisonId.ModId != 0xFFFFFFFF, "Poison is sent as temp!");

            uint32_t poisonId = modSystem.GetGameId(arEntry.ExtraPoisonId);
            if (AlchemyItem* pPoison = RTTI_CAST(TESForm::GetById(poisonId), TESForm, AlchemyItem))
            {
                ExtraPoison* pExtraPoison = Memory::Allocate<ExtraPoison>();
                *((uint64_t*)pExtraPoison) = 0x141623E50;
                pExtraPoison->next = nullptr;
                pExtraPoison->pPoison = pPoison;
                pExtraPoison->uiCount = arEntry.ExtraPoisonCount;
                pExtraData->Add(ExtraData::Poison, pExtraPoison);
            }
        }

        if (arEntry.ExtraSoulLevel > 0 && arEntry.ExtraSoulLevel <= 5)
        {
            ExtraSoul* pExtraSoul = Memory::Allocate<ExtraSoul>();
            *((uint64_t*)pExtraSoul) = 0x141627220;
            pExtraSoul->next = nullptr;
            pExtraSoul->cSoul = static_cast<SOUL_LEVEL>(arEntry.ExtraSoulLevel);
            pExtraData->Add(ExtraData::Soul, pExtraSoul);
        }

        if (!arEntry.ExtraTextDisplayName.empty())
        {
            ExtraTextDisplayData* pExtraText = Memory::Allocate<ExtraTextDisplayData>();
            *((uint64_t*)pExtraText) = 0x1416244D0;
            pExtraText->next = nullptr;
            pExtraText->DisplayName = arEntry.ExtraTextDisplayName.c_str();
            pExtraText->usCustomNameLength = arEntry.ExtraTextDisplayName.length();
            pExtraText->iOwnerInstance = -2;
            pExtraText->fTemperFactor = 1.0F;
            pExtraData->Add(ExtraData::TextDisplayData, pExtraText);
        }

        if (arEntry.ExtraWorn)
        {
            ExtraWorn* pExtraWorn = Memory::Allocate<ExtraWorn>();
            *((uint64_t*)pExtraWorn) = 0x1416239F0;
            pExtraWorn->next = nullptr;
            pExtraData->Add(ExtraData::Worn, pExtraWorn);
        }

        if (arEntry.ExtraWornLeft)
        {
            ExtraWornLeft* pExtraWornLeft = Memory::Allocate<ExtraWornLeft>();
            *((uint64_t*)pExtraWornLeft) = 0x141623A10;
            pExtraWornLeft->next = nullptr;
            pExtraData->Add(ExtraData::WornLeft, pExtraWornLeft);
        }

        if (pExtraData->data == nullptr)
        {
            Memory::Delete(pExtraData);
            pExtraData = nullptr;
        }
    }

    /*
    BSExtraDataList* pExtraData2 = pExtraData;
    if (pExtraData)
        DebugBreak();
    */

    AddObjectToContainer(pObject, pExtraData, arEntry.Count, nullptr);
    spdlog::info("Added object to container, form id: {:X}, extra data count: {}, entry count: {}", pObject->formID,
                 pExtraData ? pExtraData->GetCount() : -1, arEntry.Count);
}

void TESObjectREFR::Activate(TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* aObjectToGet, int32_t aCount, char aDefaultProcessing) noexcept
{
    return ThisCall(RealActivate, this, apActivator, aUnk1, aObjectToGet, aCount, aDefaultProcessing);
}

void TESObjectREFR::EnableImpl() noexcept
{
    TP_THIS_FUNCTION(TEnableImpl, void, TESObjectREFR, bool aResetInventory);

    POINTER_SKYRIMSE(TEnableImpl, s_enable, 0x1402AA780 - 0x140000000);
    
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

bool TP_MAKE_THISCALL(HookPlayAnimationAndWait, void, uint32_t auiStackID, TESObjectREFR* apSelf, BSFixedString* apAnimation, BSFixedString* apEventName)
{
    spdlog::debug("Animation: {}, EventName: {}", apAnimation->AsAscii(), apEventName->AsAscii());

    if (!s_cancelAnimationWaitEvent && (apSelf->formID < 0xFF000000))
        World::Get().GetRunner().Trigger(ScriptAnimationEvent(apSelf->formID, apAnimation->AsAscii(), apEventName->AsAscii()));

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

    if (!s_cancelAnimationEvent && (apSelf->formID < 0xFF000000))
        World::Get().GetRunner().Trigger(ScriptAnimationEvent(apSelf->formID, String{}, apEventName->AsAscii()));

    return ThisCall(RealPlayAnimation, apThis, auiStackID, apSelf, apEventName);
}

void TP_MAKE_THISCALL(HookActivate, TESObjectREFR, TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* apObjectToGet, int32_t aCount, char aDefaultProcessing)
{
    auto* pActivator = RTTI_CAST(apActivator, TESObjectREFR, Actor);
    if (pActivator)
        World::Get().GetRunner().Trigger(ActivateEvent(apThis, pActivator, apObjectToGet, aUnk1, aCount, aDefaultProcessing));

    return ThisCall(RealActivate, apThis, apActivator, aUnk1, apObjectToGet, aCount, aDefaultProcessing);
}

void* TP_MAKE_THISCALL(HookAddInventoryItem, TESObjectREFR, TESBoundObject* apItem, BSExtraDataList* apExtraData, uint32_t aCount, TESObjectREFR* apOldOwner)
{
    World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID));
    return ThisCall(RealAddInventoryItem, apThis, apItem, apExtraData, aCount, apOldOwner);
}

// TODO: here's your deadlock/memory leak, fix that
void* TP_MAKE_THISCALL(HookRemoveInventoryItem, TESObjectREFR, float* apUnk0, TESBoundObject* apItem, uint32_t aCount, uint32_t aUnk1, BSExtraDataList* apExtraData, TESObjectREFR* apNewOwner, NiPoint3* apUnk2, NiPoint3* apUnk3)
{
    thread_local static uint32_t count = 0;
    count++;
    if (count > 1)
        spdlog::error("\tRecursive RemoveInventoryItem!");

    World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID));

    auto result = ThisCall(RealRemoveInventoryItem, apThis, apUnk0, apItem, aCount, aUnk1, apExtraData, apNewOwner, apUnk2, apUnk3);

    count--;

    return result;
}

static TiltedPhoques::Initializer s_objectReferencesHooks([]() {
    POINTER_SKYRIMSE(TActivate, s_activate, 0x1402A90F0 - 0x140000000);
    POINTER_SKYRIMSE(TAddInventoryItem, s_addInventoryItem, 0x1402A08A0 - 0x140000000);
    POINTER_SKYRIMSE(TRemoveInventoryItem, s_removeInventoryItem, 0x14029FC20 - 0x140000000);
    POINTER_SKYRIMSE(TPlayAnimationAndWait, s_playAnimationAndWait, 0x1409BD880 - 0x140000000);
    POINTER_SKYRIMSE(TPlayAnimation, s_playAnimation, 0x1409BD800 - 0x140000000);

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
