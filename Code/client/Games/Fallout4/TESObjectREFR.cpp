#include <TiltedOnlinePCH.h>
#include <Games/References.h>
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
    BSScript::Internal::AssociatedScript scripts[6];

    auto* pVM = GameVM::Get()->virtualMachine;

    pVM->scriptsLock.Lock();
    auto* pObjectHandlePolicy = pVM->GetObjectHandlePolicy();
    auto objectHandle = pObjectHandlePolicy->GetHandle(formType, this);

    uint32_t crc = CRC32::GenerateCRC(objectHandle);

    Vector<BSScript::Object*> objects;

    auto* pScriptTableEntry = pVM->scriptsMap.GetEntry(crc, objectHandle);
    if (pScriptTableEntry)
    {
        /*
        BSScript::Internal::AssociatedScript singularScript;
        if (pScriptTableEntry->value.size == 1)
        {
            auto* pObject = (BSScript::Object*)((uint64_t)pScriptTableEntry->value.externalScript & 0xFFFFFFFFFFFFFFFE);
            singularScript.pointerOrFlags = (uint64_t)pObject;
            if (pObject)
                pObject->IncreaseRef();
        }
        */

        scripts[0].pointerOrFlags = (uint64_t)pScriptTableEntry->value.externalScript;

        for (uint32_t i = 0; i < pScriptTableEntry->value.size; ++i)
        {
            auto* pObject = (BSScript::Object*)(scripts[i].pointerOrFlags & 0xFFFFFFFFFFFFFFFE);
            objects.push_back(pObject);
        }

        /*
        if (pScriptTableEntry->value.size == 1)
            singularScript.Cleanup(0);
        */
    }

    if (&pVM->scriptsLock)
        pVM->scriptsLock.Unlock();

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

            BSScript::Variable variable;
            variable.type = BSScript::Variable::Type::kEmpty;
            variable.data.i = 0;
            variable.data.s = nullptr;
            variable.data.f = 0;
            variable.data.b = 0;
            
            pVM->GetVariable(&object, index, &variable);

            char buffer[256];
            variable.ConvertToString(buffer, 256, true, true);
            spdlog::info("\tVariable: {}: {}", currentVar->AsAscii(), buffer);

            currentVar += 3;
        }
    }
}
