#include <TiltedOnlinePCH.h>

#include <Misc/BSScript.h>

#include <Games/References.h>
#include <Misc/GameVM.h>

BSScript::Variable::~Variable() noexcept
{
    Reset();
}

void BSScript::Variable::Reset() noexcept
{
    TP_THIS_FUNCTION(TReset, void, BSScript::Variable);

    POINTER_FALLOUT4(TReset, s_reset, 0x1426F0960 - 0x140000000);

    ThisCall(s_reset, this);
}

void BSScript::Variable::Clear() noexcept
{
    Reset();

    VarType.uData.eRawType = TypeInfo::NONE;
}

template <> void BSScript::Variable::Set(int32_t aValue) noexcept
{
    Reset();

    VarType.uData.eRawType = TypeInfo::INT;
    uiValue.i = aValue;
}

template <> void BSScript::Variable::Set(float aValue) noexcept
{
    Reset();

    VarType.uData.eRawType = TypeInfo::FLOAT;
    uiValue.f = aValue;
}

template <> void BSScript::Variable::Set(bool aValue) noexcept
{
    Reset();

    VarType.uData.eRawType = TypeInfo::BOOL;
    uiValue.b = aValue;
}

template <> void BSScript::Variable::Set(const char* acpValue) noexcept
{
    Reset();

    VarType.uData.eRawType = TypeInfo::STRING;
    auto pStr = reinterpret_cast<BSFixedString*>(&uiValue.s);
    pStr->Set(acpValue);
}

void BSScript::Variable::ConvertToString(char* aBuffer, uint32_t aBufferSize, bool aQuoteStringType, bool aObjectHandleOnly) noexcept
{
    TP_THIS_FUNCTION(TToString, void, BSScript::Variable, char* aBuffer, uint32_t aBufferSize, bool aQuoteStringType, bool aObjectHandleOnly);

    POINTER_FALLOUT4(TToString, s_toString, 0x1426F03E0 - 0x140000000);

    ThisCall(s_toString, this, aBuffer, aBufferSize, aQuoteStringType, aObjectHandleOnly);
}

// This is just BSTArray::SetSize
void BSScript::Statement::SetSize(uint32_t aCount) noexcept
{
    TP_THIS_FUNCTION(TSetSize, void, BSScript::Statement, uint32_t aCount);

    POINTER_FALLOUT4(TSetSize, s_setSize, 0x140551440 - 0x140000000);

    ThisCall(s_setSize, this, aCount);
}

void BSScript::Object::IncreaseRef() noexcept
{
    TP_THIS_FUNCTION(TIncreaseRef, void, BSScript::Object);

    POINTER_FALLOUT4(TIncreaseRef, s_increaseRef, 0x1426ED160 - 0x140000000);

    ThisCall(s_increaseRef, this);
}

void BSScript::Object::DecreaseRef() noexcept
{
    TP_THIS_FUNCTION(TDecreaseRef, void, BSScript::Object);

    POINTER_FALLOUT4(TDecreaseRef, s_decreaseRef, 0x1426ED210 - 0x140000000);

    ThisCall(s_decreaseRef, this);
}

int64_t BSScript::ObjectTypeInfo::GetVariableIndex(BSFixedString* aName) noexcept
{
    TP_THIS_FUNCTION(TGetVariableIndex, int64_t, BSScript::ObjectTypeInfo, BSFixedString* aName);

    POINTER_FALLOUT4(TGetVariableIndex, s_getVariableIndex, 0x1426F4780 - 0x140000000);

    return ThisCall(s_getVariableIndex, this, aName);
}

void BSScript::Internal::AssociatedScript::Cleanup(char aUnkFlag) noexcept
{
    TP_THIS_FUNCTION(TCleanup, void, BSScript::Internal::AssociatedScript);

    POINTER_FALLOUT4(TCleanup, s_cleanup, 0x142761180 - 0x140000000);

    ThisCall(s_cleanup, this);
}

void BSScript::GetObjects(Vector<BSScript::Object*>& aObjects, TESObjectREFR* aObjectRefr) noexcept
{
    BSScript::Internal::AssociatedScript* scripts = nullptr;
    BSScript::Internal::AssociatedScript scriptsArray[6];

    auto* pVM = GameVM::Get()->virtualMachine;

    auto* pObjectHandlePolicy = pVM->GetObjectHandlePolicy();
    uint64_t objectHandle = pObjectHandlePolicy->GetHandleForObject(aObjectRefr->formType, aObjectRefr);

    uint32_t crc = CRC32::GenerateCRC(objectHandle);

    auto* pScriptTableEntry = pVM->scriptsMap.GetEntry(crc, objectHandle);
    if (pScriptTableEntry)
    {
        if (pScriptTableEntry->value.size > 1)
            scripts = pScriptTableEntry->value.externalScript;
        else
        {
            scriptsArray[0].pointerOrFlags = (uint64_t)pScriptTableEntry->value.externalScript;
            scripts = scriptsArray;
        }

        for (uint32_t i = 0; i < pScriptTableEntry->value.size; ++i)
        {
            auto* pObject = (BSScript::Object*)(scripts[i].pointerOrFlags & 0xFFFFFFFFFFFFFFFE);
            aObjects.push_back(pObject);
        }
    }
}
