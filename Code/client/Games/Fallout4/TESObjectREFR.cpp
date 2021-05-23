#include <TiltedOnlinePCH.h>
#include <Games/References.h>
#include <Forms/TESNPC.h>
#include <Misc/BSScript.h>
#include <Misc/GameVM.h>

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

void TESObjectREFR::GetScriptVariables() noexcept
{
    auto* pVM = GameVM::Get()->virtualMachine;

    pVM->scriptsLock.Lock();

    Vector<BSScript::Object*> objects;
    BSScript::GetObjects(objects, this);

    for (auto object : objects)
    {
        spdlog::info("Script: {}", object->typeInfo->name.AsAscii());

        Vector<BSFixedString*> variables;

        auto* typeInfo = object->typeInfo;

        BSFixedString* currentVar = (BSFixedString*)((char*)typeInfo->data + 8 * ((typeInfo->flags1 >> 3) & 0x1F));
        BSFixedString* endVar = &currentVar[3 * ((typeInfo->flags1 >> 8) & 0x3FF)];
        while (currentVar != endVar)
        {
            variables.push_back(currentVar);
            auto index = typeInfo->GetVariableIndex(currentVar);

            BSScript::Variable variable{};

            pVM->GetVariable(&object, index, &variable);

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
        auto* typeInfo = object->typeInfo;
        if (String(typeInfo->name.AsAscii()) == aScriptName)
        {
            spdlog::info("Found script {}", typeInfo->name.AsAscii());
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

    auto* typeInfo = pObject->typeInfo;

    bool foundVariable = false;

    BSFixedString* currentVar = (BSFixedString*)((char*)typeInfo->data + 8 * ((typeInfo->flags1 >> 3) & 0x1F));
    BSFixedString* endVar = &currentVar[3 * ((typeInfo->flags1 >> 8) & 0x3FF)];
    while (currentVar != endVar)
    {
        auto index = typeInfo->GetVariableIndex(currentVar);

        BSScript::Variable variable{};

        pVM->GetVariable(&pObject, index, &variable);

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

    auto index = typeInfo->GetVariableIndex(&variableName);

    int oldValue = pObject->variables[index].data.i;
    pObject->variables[index].data.i = aNewValue;

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
        auto* typeInfo = object->typeInfo;
        if (String(typeInfo->name.AsAscii()) == aScriptName)
        {
            spdlog::info("Found script {}", typeInfo->name.AsAscii());
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

    pObject->state.Set(aState.c_str());

    spdlog::info("Updated state of script {} to {}", aScriptName, pObject->state.AsAscii());

    if (&pVM->scriptsLock)
        pVM->scriptsLock.Unlock();
}
