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

    POINTER_FALLOUT4(TReset, s_reset, 1183889);

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

void BSScript::Statement::SetSize(uint32_t aCount) noexcept
{
    TP_THIS_FUNCTION(TSetSize, void, BSScript::Statement, uint32_t aCount);

    POINTER_FALLOUT4(TSetSize, s_setSize, 1293891);

    ThisCall(s_setSize, this, aCount);
}

