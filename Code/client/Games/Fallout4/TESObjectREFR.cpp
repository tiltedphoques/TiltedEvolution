#include <TiltedOnlinePCH.h>
#include <Games/References.h>
#include <World.h>
#include <Events/ActivateEvent.h>
#include <Events/InventoryChangeEvent.h>

#include <Forms/TESNPC.h>
#include <Misc/BSScript.h>
#include <Misc/GameVM.h>

TP_THIS_FUNCTION(TActivate, void, TESObjectREFR, TESObjectREFR* apActivator, TESBoundObject* apObjectToGet, int32_t aCount, bool aDefaultProcessing, bool aFromScript, bool aIsLooping);
TP_THIS_FUNCTION(TAddInventoryItem, void, TESObjectREFR, TESBoundObject* apObject, BSExtraDataList* apExtraData, uint32_t aCount,
                 TESObjectREFR* apOldContainer, void* apUnk1, void* apUnk2);
TP_THIS_FUNCTION(TRemoveInventoryItem, uint32_t*, TESObjectREFR, uint32_t* apUnk1, void* apUnk2);

static TActivate* RealActivate = nullptr;
static TAddInventoryItem* RealAddInventoryItem = nullptr;
static TRemoveInventoryItem* RealRemoveInventoryItem = nullptr;

void TESObjectREFR::SaveInventory(BGSSaveFormBuffer* apBuffer) const noexcept
{
    TP_THIS_FUNCTION(TSaveFunc, void, void, BGSSaveFormBuffer*);

    POINTER_FALLOUT4(TSaveFunc, s_save, 0x1401ACB20 - 0x140000000);

    ThisCall(s_save, inventory, apBuffer);
}

void TESObjectREFR::LoadInventory(BGSLoadFormBuffer* apBuffer) noexcept
{
    TP_THIS_FUNCTION(TLoadFunc, void, void, BGSLoadFormBuffer*);

    POINTER_FALLOUT4(TLoadFunc, s_load, 0x1401ACC00 - 0x140000000);

    sub_A8();
    sub_A7(nullptr);

    ThisCall(s_load, inventory, apBuffer);
}

void TESObjectREFR::RemoveAllItems() noexcept
{
    using TRemoveAllItems = void(void*, void*, TESObjectREFR*, TESObjectREFR*, bool);

    POINTER_FALLOUT4(TRemoveAllItems, s_removeAllItems, 0x14140CE90 - 0x140000000);

    s_removeAllItems(nullptr, nullptr, this, nullptr, false);
}

ActorValueInfo* TESObjectREFR::GetActorValueInfo(uint32_t aId) noexcept
{
    using TGetActorValueInfoArray = ActorValueInfo**();

    POINTER_FALLOUT4(TGetActorValueInfoArray, s_getActorValueInfoArray, 0x14006B1F0 - 0x140000000);

    ActorValueInfo** actorValueInfoArray = s_getActorValueInfoArray.Get()();

    return actorValueInfoArray[aId];
}

void TESObjectREFR::Activate(TESObjectREFR* apActivator, TESBoundObject* apObjectToGet, int32_t aCount, bool aDefaultProcessing, bool aFromScript, bool aIsLooping) noexcept
{
    return ThisCall(RealActivate, this, apActivator, apObjectToGet, aCount, aDefaultProcessing, aFromScript, aIsLooping);
}

void TP_MAKE_THISCALL(HookActivate, TESObjectREFR, TESObjectREFR* apActivator, TESBoundObject* apObjectToGet, int aCount, bool aDefaultProcessing, bool aFromScript, bool aIsLooping)
{
    auto* pActivator = RTTI_CAST(apActivator, TESObjectREFR, Actor);
    if (pActivator)
        World::Get().GetRunner().Trigger(ActivateEvent(apThis, pActivator, apObjectToGet, aCount, aDefaultProcessing, aFromScript, aIsLooping));

    return ThisCall(RealActivate, apThis, apActivator, apObjectToGet, aCount, aDefaultProcessing, aFromScript, aIsLooping);
}

void TP_MAKE_THISCALL(HookAddInventoryItem, TESObjectREFR, TESBoundObject* apObject, BSExtraDataList* apExtraData,
                      uint32_t aCount, TESObjectREFR* apOldContainer, void* apUnk1, void* apUnk2)
{
    World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID));
    ThisCall(RealAddInventoryItem, apThis, apObject, apExtraData, aCount, apOldContainer, apUnk1, apUnk2);
}

uint32_t* TP_MAKE_THISCALL(HookRemoveInventoryItem, TESObjectREFR, uint32_t* apUnk1, void* apUnk2)
{
    World::Get().GetRunner().Trigger(InventoryChangeEvent(apThis->formID));
    return ThisCall(RealRemoveInventoryItem, apThis, apUnk1, apUnk2);
}

void TESObjectREFR::GetScriptVariables() noexcept
{
    auto* pVM = GameVM::Get()->virtualMachine;

    pVM->scriptsLock.Lock();

    Vector<BSScript::Object*> objects;
    BSScript::GetObjects(objects, this);

    for (auto object : objects)
    {
        spdlog::info("Script: {}", object->pType->sName.AsAscii());

        Vector<BSFixedString*> variables;

        auto* pType = object->pType;

        // TODO: this can be cleaned up further
        BSFixedString* currentVar = (BSFixedString*)((char*)pType->pData + 8 * pType->uiUserFlagCount);
        BSFixedString* endVar = &currentVar[3 * pType->uiVariableCount];
        while (currentVar != endVar)
        {
            variables.push_back(currentVar);
            auto index = pType->GetVariableIndex(currentVar);

            BSScript::Variable variable{};

            pVM->GetVariableValue(&object, index, &variable);

            char buffer[256];
            variable.ConvertToString(buffer, 256, true, true);
            spdlog::info("\tVariable: {} at index {}: {}", currentVar->AsAscii(), index, buffer);

            currentVar += 3;
        }
    }

    if (&pVM->scriptsLock)
        pVM->scriptsLock.Unlock();
}

void TESObjectREFR::SetScriptVariable(const String aScriptName, const String aVariableName, int aNewValue) noexcept
{
    auto* pVM = GameVM::Get()->virtualMachine;

    pVM->scriptsLock.Lock();

    Vector<BSScript::Object*> objects;
    BSScript::GetObjects(objects, this);

    BSScript::Object* pObject = nullptr;

    for (auto object : objects)
    {
        auto* pType = object->pType;
        if (String(pType->sName.AsAscii()) == aScriptName)
        {
            spdlog::info("Found script {}", pType->sName.AsAscii());
            pObject = object;
            break;
        }
    }

    if (!pObject)
    {
        spdlog::warn("SetScriptVariable: script not found: {}", aScriptName);
        if (&pVM->scriptsLock)
            pVM->scriptsLock.Unlock();
        return;
    }

    auto* pType = pObject->pType;

    bool foundVariable = false;

    BSFixedString* currentVar = (BSFixedString*)((char*)pType->pData + 8 * pType->uiUserFlagCount);
    BSFixedString* endVar = &currentVar[3 * pType->uiVariableCount];
    while (currentVar != endVar)
    {
        auto index = pType->GetVariableIndex(currentVar);

        BSScript::Variable variable{};

        pVM->GetVariableValue(&pObject, index, &variable);

        if (String(currentVar->AsAscii()) == aVariableName)
        {
            foundVariable = true;
            break;
        }

        currentVar += 3;
    }

    if (!foundVariable)
    {
        spdlog::warn("SetScriptVariable: variable not found: {}", aVariableName);
        if (&pVM->scriptsLock)
            pVM->scriptsLock.Unlock();
        return;
    }

    // TODO: remove this, just use currentVar
    BSFixedString variableName(aVariableName.c_str());

    auto index = pType->GetVariableIndex(&variableName);

    int oldValue = pObject->variables[index].uiValue.i;
    pObject->variables[index].uiValue.i = aNewValue;

    spdlog::info("Successfully changed script {} variable {} from {} to {}", aScriptName, aVariableName, oldValue,
                 aNewValue);

    variableName.Release();

    if (&pVM->scriptsLock)
        pVM->scriptsLock.Unlock();
}

void TESObjectREFR::SetScriptState(const String aScriptName, const String aState) noexcept
{
    auto* pVM = GameVM::Get()->virtualMachine;

    pVM->scriptsLock.Lock();

    Vector<BSScript::Object*> objects;
    BSScript::GetObjects(objects, this);

    BSScript::Object* pObject = nullptr;

    for (auto object : objects)
    {
        auto* pType = object->pType;
        if (String(pType->sName.AsAscii()) == aScriptName)
        {
            spdlog::info("Found script {}", pType->sName.AsAscii());
            pObject = object;
            break;
        }
    }

    if (!pObject)
    {
        spdlog::warn("SetScriptState: script not found: {}", aScriptName);
        if (&pVM->scriptsLock)
            pVM->scriptsLock.Unlock();
        return;
    }

    pObject->sCurrentState.Set(aState.c_str());

    spdlog::info("Updated state of script {} to {}", aScriptName, pObject->sCurrentState.AsAscii());

    if (&pVM->scriptsLock)
        pVM->scriptsLock.Unlock();
}

static TiltedPhoques::Initializer s_objectReferencesHooks([]() {
        POINTER_FALLOUT4(TActivate, s_activate, 0x14040C750 - 0x140000000);
        POINTER_FALLOUT4(TAddInventoryItem, s_addItem, 0x1403FBB00 - 0x140000000);
        POINTER_FALLOUT4(TRemoveInventoryItem, s_removeItem, 0x1403F9830 - 0x140000000);

        RealActivate = s_activate.Get();
        RealAddInventoryItem = s_addItem.Get();
        RealRemoveInventoryItem = s_removeItem.Get();

        TP_HOOK(&RealActivate, HookActivate);
        TP_HOOK(&RealAddInventoryItem, HookAddInventoryItem);
        TP_HOOK(&RealRemoveInventoryItem, HookRemoveInventoryItem);
});

