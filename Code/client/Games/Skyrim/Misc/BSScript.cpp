#include <Misc/BSScript.h>

uint64_t BSScript::Object::GetHandle()
{
    TP_THIS_FUNCTION(TGetHandle, uint64_t, BSScript::Object);
    POINTER_SKYRIMSE(TGetHandle, s_getHandle, 104247);
    return ThisCall(s_getHandle, this);
}

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

    POINTER_SKYRIMSE(TReset, s_reset, 104296);

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
    auto pStr = (BSFixedString*)&data.s;
    pStr->Set(acpValue);
}

BSScript::Object* BSScript::Variable::GetObject() const noexcept
{
    if (type > Type::kMax)
    {
        bool isArray = type & 1;
        if (isArray)
            return nullptr;
    }
    else if (type != Type::kObject)
        return nullptr;

    return data.pObj;
}

void BSScript::IFunctionArguments::Statement::SetSize(uint32_t aCount) noexcept
{
    TP_THIS_FUNCTION(TSetSize, void, BSScript::IFunctionArguments::Statement, uint32_t aCount);

    POINTER_SKYRIMSE(TSetSize, s_setSize, 53915);

    ThisCall(s_setSize, this, aCount);
}

BSScript::IObjectHandlePolicy* BSScript::IObjectHandlePolicy::Get() noexcept
{
    POINTER_SKYRIMSE(BSScript::IObjectHandlePolicy*, s_policy, 414391);

    return *s_policy.Get();
}
