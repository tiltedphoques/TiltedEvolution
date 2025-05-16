#include <TiltedOnlinePCH.h>

#include <Games/References.h>
#include <Games/Overrides.h>

#include <World.h>
#include <Services/PapyrusService.h>
#include <Events/ActivateEvent.h>
#include <Events/InventoryChangeEvent.h>
#include <Events/ScriptAnimationEvent.h>
#include <Events/LockChangeEvent.h>

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
#include <BSAnimationGraphManager.h>
#include <Havok/BShkbAnimationGraph.h>
#include <Havok/hkbBehaviorGraph.h>
#include <Havok/hkbVariableValueSet.h>
#include <Havok/hkbStateMachine.h>
#include <Forms/TESObjectCELL.h>
#include <Forms/TESWorldSpace.h>

#include <Structs/AnimationGraphDescriptorManager.h>
#include <Structs/AnimationVariables.h>

extern const AnimationGraphDescriptor* BehaviorVarPatch(BSAnimationGraphManager* pManager, Actor* pActor);

TP_THIS_FUNCTION(TActivate, bool, TESObjectREFR, TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* apObjectToGet, int32_t aCount, char aDefaultProcessing);
TP_THIS_FUNCTION(TAddInventoryItem, void, TESObjectREFR, TESBoundObject* apItem, ExtraDataList* apExtraData, int32_t aCount, TESObjectREFR* apOldOwner);
TP_THIS_FUNCTION(
    TRemoveInventoryItem, BSPointerHandle<TESObjectREFR>*, TESObjectREFR, BSPointerHandle<TESObjectREFR>* apResult, TESBoundObject* apItem, int32_t aCount, ITEM_REMOVE_REASON aReason, ExtraDataList* apExtraList, TESObjectREFR* apMoveToRef, const NiPoint3* apDropLoc, const NiPoint3* apRotate);
TP_THIS_FUNCTION(TPlayAnimationAndWait, bool, void, uint32_t auiStackID, TESObjectREFR* apSelf, BSFixedString* apAnimation, BSFixedString* apEventName);
TP_THIS_FUNCTION(TPlayAnimation, bool, void, uint32_t auiStackID, TESObjectREFR* apSelf, BSFixedString* apEventName);
TP_THIS_FUNCTION(TRotate, void, TESObjectREFR, float aAngle);
TP_THIS_FUNCTION(TLockChange, void, TESObjectREFR);

static TActivate* RealActivate = nullptr;
static TAddInventoryItem* RealAddInventoryItem = nullptr;
static TRemoveInventoryItem* RealRemoveInventoryItem = nullptr;
static TPlayAnimationAndWait* RealPlayAnimationAndWait = nullptr;
static TPlayAnimation* RealPlayAnimation = nullptr;
static TRotate* RealRotateX = nullptr;
static TRotate* RealRotateY = nullptr;
static TRotate* RealRotateZ = nullptr;
static TLockChange* RealLockChange = nullptr;

#ifdef SAVE_STUFF

#include <Games/Skyrim/SaveLoad.h>

void TESObjectREFR::Save_Reversed(const uint32_t aChangeFlags, Buffer::Writer& aWriter)
{
    TESForm::Save_Reversed(aChangeFlags, aWriter);

    if (aChangeFlags & CHANGE_REFR_BASEOBJECT)
    {
        // save baseForm->formID;
        // we don't because each player has it's own form id system
    }

    if (aChangeFlags & CHANGE_REFR_SCALE)
    {
        // So skyrim does some weird conversion shit here, we are going to do the same for now
        float fScale = scale;
        fScale /= 100.f;
        aWriter.WriteBytes((uint8_t*)&fScale, 4);
    }

    // So skyrim
    uint32_t extraFlags = 0xA6021C40;
    if (formType == Character)
        extraFlags = 0xA6061840;

    if (aChangeFlags & extraFlags)
    {
        // We have flags to save
    }

    if (aChangeFlags & (CHANGE_REFR_INVENTORY | CHANGE_REFR_LEVELED_INVENTORY))
    {
    }

    if (aChangeFlags & CHANGE_REFR_ANIMATION)
    {
        // do something with animations
        // get extradata 0x41
    }
}

#endif

TESObjectREFR* TESObjectREFR::GetByHandle(uint32_t aHandle) noexcept
{
    TESObjectREFR* pResult = nullptr;

    using TGetRefrByHandle = void(uint32_t & aHandle, TESObjectREFR * &apResult);

    POINTER_SKYRIMSE(TGetRefrByHandle, s_getRefrByHandle, 17201);

    s_getRefrByHandle.Get()(aHandle, pResult);

    if (pResult)
        pResult->handleRefObject.DecRefHandle();

    return pResult;
}

BSPointerHandle<TESObjectREFR> TESObjectREFR::GetHandle() const noexcept
{
    TP_THIS_FUNCTION(TGetHandle, BSPointerHandle<TESObjectREFR>, const TESObjectREFR, BSPointerHandle<TESObjectREFR>* apResult);
    POINTER_SKYRIMSE(TGetHandle, s_getHandle, 19846);

    BSPointerHandle<TESObjectREFR> result{};
    TiltedPhoques::ThisCall(s_getHandle, this, &result);

    return result;
}

uint32_t* TESObjectREFR::GetNullHandle() noexcept
{
    POINTER_SKYRIMSE(uint32_t, s_nullHandle, 400312);

    return s_nullHandle.Get();
}

void TESObjectREFR::SetRotation(float aX, float aY, float aZ) noexcept
{
    TiltedPhoques::ThisCall(RealRotateX, this, aX);
    TiltedPhoques::ThisCall(RealRotateY, this, aY);
    TiltedPhoques::ThisCall(RealRotateZ, this, aZ);
}

using TiltedPhoques::Serialization;

void TESObjectREFR::SaveAnimationVariables(AnimationVariables& aVariables) const noexcept
{
    BSAnimationGraphManager* pManager = nullptr;
    if (animationGraphHolder.GetBSAnimationGraph(&pManager))
    {
        BSScopedLock<BSRecursiveLock> _{pManager->lock};

        if (pManager->animationGraphIndex < pManager->animationGraphs.size)
        {
            auto* pActor = Cast<Actor>(this);
            if (!pActor)
                return;

            const BShkbAnimationGraph* pGraph = nullptr;

            if (pActor->formID == 0x14)
                pGraph = pManager->animationGraphs.Get(0);
            else
                pGraph = pManager->animationGraphs.Get(pManager->animationGraphIndex);

            if (!pGraph)
                return;

            if (!pGraph->behaviorGraph || !pGraph->behaviorGraph->stateMachine || !pGraph->behaviorGraph->stateMachine->name)
                return;

            auto* pExtendedActor = pActor->GetExtension();
            if (pExtendedActor->GraphDescriptorHash == 0)
            {
                // Force third person graph to be used on player
                if (pActor->formID == 0x14)
                    pExtendedActor->GraphDescriptorHash = pManager->GetDescriptorKey(0);
                else
                    pExtendedActor->GraphDescriptorHash = pManager->GetDescriptorKey();
            }

            auto pDescriptor = AnimationGraphDescriptorManager::Get().GetDescriptor(pExtendedActor->GraphDescriptorHash);

            // Modded behavior check if descriptor wasn't found
            if (!pDescriptor)
                pDescriptor = BehaviorVarPatch(pManager, pActor);

            if (!pDescriptor)
                return;

            const auto* pVariableSet = pGraph->behaviorGraph->animationVariables;

            if (!pVariableSet)
                return;

            aVariables.Booleans.assign(pDescriptor->BooleanLookUpTable.size(), false);
            aVariables.Floats.assign(pDescriptor->FloatLookupTable.size(), 0.f);
            aVariables.Integers.assign(pDescriptor->IntegerLookupTable.size(), 0);

            for (size_t i = 0; i < pDescriptor->BooleanLookUpTable.size(); ++i)
            {
                const auto idx = pDescriptor->BooleanLookUpTable[i];

                if (pVariableSet->data[idx] != 0)
                    aVariables.Booleans[i] = true;
            }

            for (size_t i = 0; i < pDescriptor->FloatLookupTable.size(); ++i)
            {
                const auto idx = pDescriptor->FloatLookupTable[i];

                aVariables.Floats[i] = *reinterpret_cast<float*>(&pVariableSet->data[idx]);
            }

            for (size_t i = 0; i < pDescriptor->IntegerLookupTable.size(); ++i)
            {
                const auto idx = pDescriptor->IntegerLookupTable[i];

                aVariables.Integers[i] = *reinterpret_cast<uint32_t*>(&pVariableSet->data[idx]);
            }
        }

        pManager->Release();
    }
}

void TESObjectREFR::LoadAnimationVariables(const AnimationVariables& aVariables) const noexcept
{
    BSAnimationGraphManager* pManager = nullptr;
    if (animationGraphHolder.GetBSAnimationGraph(&pManager))
    {
        BSScopedLock<BSRecursiveLock> _{pManager->lock};

        if (pManager->animationGraphIndex < pManager->animationGraphs.size)
        {
            const auto* pGraph = pManager->animationGraphs.Get(pManager->animationGraphIndex);

            if (!pGraph)
                return;

            if (!pGraph->behaviorGraph || !pGraph->behaviorGraph->stateMachine || !pGraph->behaviorGraph->stateMachine->name)
                return;

            auto* pActor = Cast<Actor>(this);
            if (!pActor)
                return;

            auto* pExtendedActor = pActor->GetExtension();
            if (pExtendedActor->GraphDescriptorHash == 0)
                pExtendedActor->GraphDescriptorHash = pManager->GetDescriptorKey();

            auto pDescriptor = AnimationGraphDescriptorManager::Get().GetDescriptor(pExtendedActor->GraphDescriptorHash);

            // Modded behavior check if descriptor wasn't found
            if (!pDescriptor)
                pDescriptor = BehaviorVarPatch(pManager, pActor);

            if (!pDescriptor)
                return;

            const auto* pVariableSet = pGraph->behaviorGraph->animationVariables;

            if (!pVariableSet)
                return;

            for (size_t i = 0; i < pDescriptor->BooleanLookUpTable.size(); ++i)
            {
                const auto idx = pDescriptor->BooleanLookUpTable[i];

                if (pVariableSet->size > idx)
                {
                    pVariableSet->data[idx] = aVariables.Booleans.size() > i ? aVariables.Booleans[i] : false;
                }
            }

            for (size_t i = 0; i < pDescriptor->FloatLookupTable.size(); ++i)
            {
                const auto idx = pDescriptor->FloatLookupTable[i];

                *reinterpret_cast<float*>(&pVariableSet->data[idx]) = aVariables.Floats.size() > i ? aVariables.Floats[i] : 0.f;
            }

            for (size_t i = 0; i < pDescriptor->IntegerLookupTable.size(); ++i)
            {
                const auto idx = pDescriptor->IntegerLookupTable[i];

                *reinterpret_cast<uint32_t*>(&pVariableSet->data[idx]) = aVariables.Integers.size() > i ? aVariables.Integers[i] : 0;
            }
        }

        pManager->Release();
    }
}

uint32_t TESObjectREFR::GetCellId() const noexcept
{
    if (!parentCell)
        return 0;

    const auto* pWorldSpace = parentCell->worldspace;

    return pWorldSpace != nullptr ? pWorldSpace->formID : parentCell->formID;
}

TESWorldSpace* TESObjectREFR::GetWorldSpace() const noexcept
{
    auto* pParentCell = parentCell ? parentCell : GetParentCell();
    if (pParentCell && !(pParentCell->cellFlags[0] & 1))
        return pParentCell->worldspace;

    return nullptr;
}

ExtraDataList* TESObjectREFR::GetExtraDataList() noexcept
{
    return &extraData;
}

// Delete() should only be used on temporaries
void TESObjectREFR::Delete() const noexcept
{
    using ObjectReference = TESObjectREFR;

    PAPYRUS_FUNCTION(void, ObjectReference, Delete);

    s_pDelete(this);
}

void TESObjectREFR::Disable() const noexcept
{
    using ObjectReference = TESObjectREFR;

    PAPYRUS_FUNCTION(void, ObjectReference, Disable, bool);

    s_pDisable(this, true);
}

void TESObjectREFR::Enable() const noexcept
{
    using ObjectReference = TESObjectREFR;

    PAPYRUS_FUNCTION(void, ObjectReference, Enable, bool);

    s_pEnable(this, true);
}

// Skyrim: MoveTo() can fail, causing the object to be deleted
void TESObjectREFR::MoveTo(TESObjectCELL* apCell, const NiPoint3& acPosition) const noexcept
{
    ScopedReferencesOverride recursionGuard;

    TP_THIS_FUNCTION(TInternalMoveTo, bool, const TESObjectREFR, uint32_t*&, TESObjectCELL*, TESWorldSpace*, const NiPoint3&, const NiPoint3&);

    POINTER_SKYRIMSE(TInternalMoveTo, s_internalMoveTo, 56626);

    TiltedPhoques::ThisCall(s_internalMoveTo, this, GetNullHandle(), apCell, apCell->worldspace, acPosition, rotation);
}

void TESObjectREFR::PayGold(int32_t aAmount) noexcept
{
    ScopedInventoryOverride _;
    PayGoldToContainer(nullptr, aAmount);
}

void TESObjectREFR::PayGoldToContainer(TESObjectREFR* pContainer, int32_t aAmount) noexcept
{
    TP_THIS_FUNCTION(TPayGoldToContainer, void, TESObjectREFR, TESObjectREFR*, int32_t);
    POINTER_SKYRIMSE(TPayGoldToContainer, s_payGoldToContainer, 37511);
    TiltedPhoques::ThisCall(s_payGoldToContainer, this, pContainer, aAmount);
}

Lock* TESObjectREFR::GetLock() const noexcept
{
    TP_THIS_FUNCTION(TGetLock, Lock*, const TESObjectREFR);
    POINTER_SKYRIMSE(TGetLock, realGetLock, 20223);

    return TiltedPhoques::ThisCall(realGetLock, this);
}

Lock* TESObjectREFR::CreateLock() noexcept
{
    TP_THIS_FUNCTION(TCreateLock, Lock*, TESObjectREFR);
    POINTER_SKYRIMSE(TCreateLock, realCreateLock, 20221);

    return TiltedPhoques::ThisCall(realCreateLock, this);
}

void TESObjectREFR::LockChange() noexcept
{
    TiltedPhoques::ThisCall(RealLockChange, this);
}

const float TESObjectREFR::GetHeight() noexcept
{
    auto boundMax = GetBoundMax();
    return boundMax.z - GetBoundMin().z;
}

TESObjectREFR::OpenState TESObjectREFR::GetOpenState() noexcept
{
    using ObjectReference = TESObjectREFR;

    PAPYRUS_FUNCTION(TESObjectREFR::OpenState, ObjectReference, GetOpenState);

    return s_pGetOpenState(this);
}

ExtraContainerChanges::Data* TESObjectREFR::GetContainerChanges() const noexcept
{
    TP_THIS_FUNCTION(TGetContainterChanges, ExtraContainerChanges::Data*, const TESObjectREFR);

    POINTER_SKYRIMSE(TGetContainterChanges, s_getContainerChangs, 16040);

    return TiltedPhoques::ThisCall(s_getContainerChangs, this);
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

    return TiltedPhoques::ThisCall(s_getContainer, this);
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

        pExtraDataList->SetEnchantmentData(pEnchantment, arEntry.ExtraEnchantCharge, arEntry.ExtraEnchantRemoveUnequip);
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
        auto duplicate = std::find_if(minimizedExtraInventory.Entries.begin(), minimizedExtraInventory.Entries.end(), [entry](const Inventory::Entry& newEntry) { return newEntry.CanBeMerged(entry); });

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

        auto duplicate = std::find_if(inventory.Entries.begin(), inventory.Entries.end(), [entry](const Inventory::Entry& newEntry) { return newEntry.CanBeMerged(entry); });

        if (duplicate == std::end(inventory.Entries))
            continue;

        entry.Count += duplicate->Count;
        duplicate->Count = 0;
    }

    spdlog::debug("MinExtraInventory count after: {}", minimizedExtraInventory.Entries.size());

    inventory.Entries.insert(inventory.Entries.end(), minimizedExtraInventory.Entries.begin(), minimizedExtraInventory.Entries.end());

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
        spdlog::warn("{}: Object to add not found, {:X}:{:X}.", __FUNCTION__, arEntry.BaseId.ModId, arEntry.BaseId.BaseId);
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
    TiltedPhoques::ThisCall(updateItemList, this, pUnkForm);
}

bool TESObjectREFR::Activate(TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* aObjectToGet, int32_t aCount, char aDefaultProcessing) noexcept
{
    ScopedActivateOverride _;

    return TiltedPhoques::ThisCall(RealActivate, this, apActivator, aUnk1, aObjectToGet, aCount, aDefaultProcessing);
}

void TESObjectREFR::EnableImpl() noexcept
{
    TP_THIS_FUNCTION(TEnableImpl, void, TESObjectREFR, bool aResetInventory);

    POINTER_SKYRIMSE(TEnableImpl, s_enable, 19800);

    TiltedPhoques::ThisCall(s_enable, this, false);
}

uint32_t TESObjectREFR::GetAnimationVariableInt(BSFixedString* apVariableName) noexcept
{
    using ObjectReference = TESObjectREFR;

    PAPYRUS_FUNCTION(uint32_t, ObjectReference, GetAnimationVariableInt, BSFixedString*);

    return s_pGetAnimationVariableInt(this, apVariableName);
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

    return TiltedPhoques::ThisCall(RealPlayAnimationAndWait, apThis, auiStackID, apSelf, apAnimation, apEventName);
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

bool TESObjectREFR::SendAnimationEvent(BSFixedString* apEventName) noexcept
{
    return animationGraphHolder.SendAnimationEvent(apEventName);
}

bool TP_MAKE_THISCALL(HookPlayAnimation, void, uint32_t auiStackID, TESObjectREFR* apSelf, BSFixedString* apEventName)
{
    spdlog::debug("EventName: {}", apEventName->AsAscii());

#if OBJECT_ANIM_SYNC
    if (!s_cancelAnimationEvent && (apSelf->formID < 0xFF000000))
        World::Get().GetRunner().Trigger(ScriptAnimationEvent(apSelf->formID, String{}, apEventName->AsAscii()));
#endif

    return TiltedPhoques::ThisCall(RealPlayAnimation, apThis, auiStackID, apSelf, apEventName);
}

bool TP_MAKE_THISCALL(HookActivate, TESObjectREFR, TESObjectREFR* apActivator, uint8_t aUnk1, TESBoundObject* apObjectToGet, int32_t aCount, char aDefaultProcessing)
{
    Actor* pActivator = Cast<Actor>(apActivator);

    // Exclude books from activation since only reading them removes them from the cell
    // Note: Books are now unsynced 
    if (pActivator && apThis->baseForm->formType != FormType::Book)
    {
        auto openState = TESObjectREFR::kNone;
        if (apThis->baseForm->formType == FormType::Door)
            openState = apThis->GetOpenState();

        World::Get().GetRunner().Trigger(
            ActivateEvent(apThis, pActivator, apObjectToGet, aCount, aDefaultProcessing, aUnk1, openState)
        );
    }

    return TiltedPhoques::ThisCall(RealActivate, apThis, apActivator, aUnk1, apObjectToGet, aCount, aDefaultProcessing);
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

    TiltedPhoques::ThisCall(RealAddInventoryItem, apThis, apItem, apExtraData, aCount, apOldOwner);
}

BSPointerHandle<TESObjectREFR>*
TP_MAKE_THISCALL(HookRemoveInventoryItem, TESObjectREFR, BSPointerHandle<TESObjectREFR>* apResult, TESBoundObject* apItem, int32_t aCount, ITEM_REMOVE_REASON aReason, ExtraDataList* apExtraList, TESObjectREFR* apMoveToRef, const NiPoint3* apDropLoc, const NiPoint3* apRotate)
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

    return TiltedPhoques::ThisCall(RealRemoveInventoryItem, apThis, apResult, apItem, aCount, aReason, apExtraList, apMoveToRef, apDropLoc, apRotate);
}

void TP_MAKE_THISCALL(HookRotateX, TESObjectREFR, float aAngle)
{
    if (apThis->formType == Actor::Type)
    {
        const auto pActor = static_cast<Actor*>(apThis);
        // We don't allow remotes to move
        if (pActor->GetExtension()->IsRemote())
            return;
    }

    return TiltedPhoques::ThisCall(RealRotateX, apThis, aAngle);
}

void TP_MAKE_THISCALL(HookRotateY, TESObjectREFR, float aAngle)
{
    if (apThis->formType == Actor::Type)
    {
        const auto pActor = static_cast<Actor*>(apThis);
        // We don't allow remotes to move
        if (pActor->GetExtension()->IsRemote())
            return;
    }

    return TiltedPhoques::ThisCall(RealRotateY, apThis, aAngle);
}

void TP_MAKE_THISCALL(HookRotateZ, TESObjectREFR, float aAngle)
{
    if (apThis->formType == Actor::Type)
    {
        const auto pActor = static_cast<Actor*>(apThis);
        // We don't allow remotes to move
        if (pActor->GetExtension()->IsRemote())
            return;
    }

    return TiltedPhoques::ThisCall(RealRotateZ, apThis, aAngle);
}

void TP_MAKE_THISCALL(HookLockChange, TESObjectREFR)
{
    TiltedPhoques::ThisCall(RealLockChange, apThis);
    const auto* pLock = apThis->GetLock();
    if(pLock)
        World::Get().GetRunner().Trigger(LockChangeEvent(apThis->formID, pLock->IsLocked(), pLock->lockLevel));
    else
        World::Get().GetRunner().Trigger(LockChangeEvent(apThis->formID, false, 0));
}

static TiltedPhoques::Initializer s_objectReferencesHooks(
    []()
    {
        POINTER_SKYRIMSE(TLockChange, s_lockChange, 19512);
        POINTER_SKYRIMSE(TRotate, s_rotateX, 19787);
        POINTER_SKYRIMSE(TRotate, s_rotateY, 19788);
        POINTER_SKYRIMSE(TRotate, s_rotateZ, 19789);
        POINTER_SKYRIMSE(TActivate, s_activate, 19796);
        POINTER_SKYRIMSE(TAddInventoryItem, s_addInventoryItem, 19708);
        POINTER_SKYRIMSE(TRemoveInventoryItem, s_removeInventoryItem, 19689);
        POINTER_SKYRIMSE(TPlayAnimationAndWait, s_playAnimationAndWait, 56206);
        POINTER_SKYRIMSE(TPlayAnimation, s_playAnimation, 56205);

        RealLockChange = s_lockChange.Get();
        RealRotateX = s_rotateX.Get();
        RealRotateY = s_rotateY.Get();
        RealRotateZ = s_rotateZ.Get();
        RealActivate = s_activate.Get();
        RealAddInventoryItem = s_addInventoryItem.Get();
        RealRemoveInventoryItem = s_removeInventoryItem.Get();
        RealPlayAnimationAndWait = s_playAnimationAndWait.Get();
        RealPlayAnimation = s_playAnimation.Get();

        TP_HOOK(&RealLockChange, HookLockChange);
        TP_HOOK(&RealRotateX, HookRotateX);
        TP_HOOK(&RealRotateY, HookRotateY);
        TP_HOOK(&RealRotateZ, HookRotateZ);
        TP_HOOK(&RealActivate, HookActivate);
        TP_HOOK(&RealAddInventoryItem, HookAddInventoryItem);
        TP_HOOK(&RealRemoveInventoryItem, HookRemoveInventoryItem);
        TP_HOOK(&RealPlayAnimationAndWait, HookPlayAnimationAndWait);
        TP_HOOK(&RealPlayAnimation, HookPlayAnimation);
    });
