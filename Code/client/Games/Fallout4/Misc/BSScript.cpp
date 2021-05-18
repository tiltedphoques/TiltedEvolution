#include <TiltedOnlinePCH.h>

#include <Misc/BSScript.h>

BSScript::Variable::Variable()
{
}

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

    type = kEmpty;
}

template <> void BSScript::Variable::Set(int32_t aValue) noexcept
{
    Reset();

    type = kInteger;
    data.i = aValue;
}

template <> void BSScript::Variable::Set(float aValue) noexcept
{
    Reset();

    type = kFloat;
    data.f = aValue;
}

template <> void BSScript::Variable::Set(bool aValue) noexcept
{
    Reset();

    type = kBoolean;
    data.b = aValue;
}

template <> void BSScript::Variable::Set(const char* acpValue) noexcept
{
    Reset();

    type = kString;
    auto pStr = reinterpret_cast<BSFixedString*>(&data.s);
    pStr->Set(acpValue);
}

void BSScript::Variable::ConvertToString(char* aBuffer, uint32_t aBufferSize, bool aQuoteStringType, bool aObjectHandleOnly) noexcept
{
    TP_THIS_FUNCTION(TToString, void, BSScript::Variable, char* aBuffer, uint32_t aBufferSize, bool aQuoteStringType, bool aObjectHandleOnly);

    POINTER_FALLOUT4(TToString, s_toString, 0x1426F03E0 - 0x140000000);

    ThisCall(s_toString, this, aBuffer, aBufferSize, aQuoteStringType, aObjectHandleOnly);
}

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

void BSScript::Internal::AssociatedScript::Cleanup(char aUnkFlag) noexcept
{
    TP_THIS_FUNCTION(TCleanup, void, BSScript::Internal::AssociatedScript);

    POINTER_FALLOUT4(TCleanup, s_cleanup, 0x142761180 - 0x140000000);

    ThisCall(s_cleanup, this);
}

int64_t BSScript::ObjectTypeInfo::GetVariableIndex(BSFixedString* name) noexcept
{
    TP_THIS_FUNCTION(TGetVariableIndex, int64_t, BSScript::ObjectTypeInfo, BSFixedString* name);

    POINTER_FALLOUT4(TGetVariableIndex, s_getVariableIndex, 0x1426F4780 - 0x140000000);

    return ThisCall(s_getVariableIndex, this, name);
}
