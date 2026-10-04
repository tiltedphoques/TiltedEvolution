#pragma once

#include <Games/Primitives.h>
#include <Misc/BSFixedString.h>

// Minimal Fallout 4 BSScript surface. Papyrus event sync ships with the F4SE
// integration; until then the VM hooks in Games/Misc/BSScript.cpp are compiled
// out and only the type surface is needed.

namespace BSScript
{
struct Object;
struct Stack;
struct StackFrame;
struct IVirtualMachine;

struct Variable
{
    enum Type : uint32_t
    {
        kNone = 0,
        kBoolean,
        kInt,
        kFloat,
        kString,
        kObject
    };

    Type GetType() const noexcept { return type; }
    Object* GetObject() const noexcept { return object; }

    union
    {
        bool b;
        int32_t i;
        float f;
        Object* object;
    };
    Type type;
    uint32_t pad;
};

struct BSIntrusiveRefCounted_ : BSIntrusiveRefCounted
{
};

struct IFunction : BSIntrusiveRefCounted_
{
    virtual ~IFunction() = default;
    virtual BSFixedString& GetName() = 0;
    virtual BSFixedString& GetObjectTypeName() = 0;
};

struct NativeFunctionBase : IFunction
{
    BSFixedString functionName;
    BSFixedString typeName;
    void* functionAddress;
    Variable::Type returnType{Variable::kNone};
};

struct IVirtualMachine
{
    virtual ~IVirtualMachine() = default;
    virtual void BindNativeMethod(NativeFunctionBase* apFunction) = 0;
};

struct NativeFunction : NativeFunctionBase
{
};

using FunctionType = bool (*)(Variable*, Variable*, Variable*, Variable*, Variable*, Variable*, Variable*, Variable*, Variable*, Variable*);

struct IsRemotePlayerFunc : NativeFunction
{
    IsRemotePlayerFunc(const char* apFunctionName, const char* apClassName, FunctionType aFunction, Variable::Type aType);
};

struct IsPlayerFunc : NativeFunction
{
    IsPlayerFunc(const char* apFunctionName, const char* apClassName, FunctionType aFunction, Variable::Type aType);
};

struct DidLaunchSkyrimTogetherFunc : NativeFunction
{
    DidLaunchSkyrimTogetherFunc(const char* apFunctionName, const char* apClassName, FunctionType aFunction, Variable::Type aType);
};
} // namespace BSScript
