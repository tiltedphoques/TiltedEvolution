#pragma once

#include <Misc/BSFixedString.h>

struct BSScript
{
    struct Variable
    {
        Variable();
        ~Variable() noexcept;

        void Reset() noexcept;
        void Clear() noexcept;

        template <class T> void Set(T aValue) noexcept
        {
            //static_assert(false);
        }

        enum Type : uint64_t
        {
            kEmpty,
            kHandle,
            kString,
            kInteger,
            kFloat,
            kBoolean
        };

        union Data 
        {
            int32_t i;
            const char* s;
            float f;
            bool b;
        };

        Type type;
        Data data;
    };

    struct Statement
    {
        void SetSize(uint32_t aCount) noexcept;

        uint8_t pad0[8];
        Variable* vars;
    };

    enum class FunctionType
    {
        kNormal = 0,
        kGetter = 1,
        kSetter = 2
    };

    enum class CallResult
    {
        kCompleted = 0,
        kSetupForVM = 1,
        kInProgress = 2,
        kFailedRetry = 3,
        kFailedAbort = 4
    };

    struct StackFrame;
    struct Stack;
    struct IVirtualMachine;

    struct IFunction : BSIntrusiveRefCounted
    {
        virtual ~IFunction() = default;
        virtual BSFixedString& GetName() = 0;
        virtual BSFixedString& GetObjectTypeName() = 0;
        virtual BSFixedString& GetStateName() = 0;
        virtual Variable::Type& GetReturnType() = 0;
        virtual uint32_t GetParamCount() = 0;
        virtual void GetParam(uint32_t aIndex, BSFixedString& apNameOut, Variable::Type& apTypeOut) = 0;
        virtual uint32_t GetStackFrameSize() = 0;
        virtual bool GetIsNative() = 0;
        virtual bool GetIsStatic() = 0;
        virtual bool GetIsEmpty() = 0;
        virtual FunctionType GetFunctionType() = 0;
        virtual uint32_t GetUserFlags() = 0;
        virtual BSFixedString& GetDocString() = 0;
        virtual void InsertLocals(StackFrame* apFrame) = 0;
        // TODO: is actually GameVM
        virtual CallResult Call(Stack* apStack, void* apLogger, IVirtualMachine* apVm, bool aArg4) = 0;
        virtual BSFixedString& GetSourceFilename() = 0;
        virtual bool TranslateIPToLineNumber(uint32_t aIndexPtr, uint32_t& aLineNumberOut) = 0;
        virtual bool GetVarNameForStackIndex(uint32_t aIndex, BSFixedString& aNameOut) = 0;
        virtual bool CanBeCalledFromTasklets() = 0;
        virtual void SetCallableFromTasklets(bool aCallable) = 0;
    };

    struct Stack
    {
        uint32_t GetPageForFrame(StackFrame* apFrame)
        {
            TP_THIS_FUNCTION(TGetPageForFrame, uint32_t, Stack, StackFrame*);
            POINTER_FALLOUT4(TGetPageForFrame, getPageForFrame, );
            return ThisCall(getPageForFrame, this, apFrame);
        }

        Variable* GetStackFrameVariable(StackFrame* apFrame, uint32_t aIndex, uint32_t aPageHint)
        {
            TP_THIS_FUNCTION(TGetStackFrameVariable, Variable*, Stack, StackFrame*, uint32_t, uint32_t);
            POINTER_FALLOUT4(TGetStackFrameVariable, getStackFrameVariable, );
            return ThisCall(getStackFrameVariable, this, apFrame, aIndex, aPageHint);
        }
    };

    struct StackFrame
    {
        uint32_t GetPageForFrame()
        {
            return pParent->GetPageForFrame(this);
        }

        Variable* GetStackFrameVariable(uint32_t aIndex, uint32_t aPageHint)
        {
            return pParent->GetStackFrameVariable(this, aIndex, aPageHint);
        }

        Stack* pParent;
    };

    struct IVirtualMachine
    {
        virtual ~IVirtualMachine() = 0;

        virtual void sub_01();
        virtual void sub_02();
        virtual void sub_03();
        virtual void sub_04();
        virtual void sub_05();
        virtual void sub_06();
        virtual void sub_07();
        virtual void GetScriptObjectType1();
        virtual void sub_09();
        virtual void sub_0A();
        virtual void sub_0B();
        virtual void sub_0C();
        virtual void sub_0D();
        virtual void sub_0E();
        virtual void sub_0F();
        virtual void sub_10();
        virtual void sub_11();
        virtual void sub_12();
        virtual void sub_13();
        virtual void sub_14();
        virtual void sub_15();
        virtual void sub_16();
        virtual void sub_17();
        virtual void sub_18();
        virtual void sub_19();
        virtual void sub_1A();
        virtual void BindNativeMethod(IFunction* apFunction);
        virtual void sub_1C();
        virtual void sub_1D();
        virtual void sub_1E();
        virtual void sub_1F();
        virtual void sub_20();
        virtual void sub_21();
        virtual void sub_22();
        virtual void sub_23();
        virtual void sub_24();
        virtual void sub_25();
        virtual void sub_26();
        virtual void sub_27();
        virtual void sub_28();
        virtual void sub_29();
        virtual void sub_2A();
        virtual void sub_2B();
        virtual void sub_2C();
        virtual void SendEvent(uint64_t aId, const BSFixedString& acEventName,
                               std::function<bool(Statement*)>& aFunctor) const noexcept;
    };

    struct NativeFunctionBase : IFunction
    {
        BSFixedString& GetName() override;
        BSFixedString& GetObjectTypeName() override;
        BSFixedString& GetStateName() override;
        Variable::Type& GetReturnType() override;
        std::uint32_t GetParamCount() override;
        void GetParam(uint32_t aIndex, BSFixedString& apNameOut, Variable::Type& a_typeOut) override;
        uint32_t GetStackFrameSize() override;
        bool GetIsNative() override;
        bool GetIsStatic() override;
        bool GetIsEmpty() override;
        FunctionType GetFunctionType() override;
        std::uint32_t GetUserFlags() override;
        BSFixedString& GetDocString() override;
        void InsertLocals(StackFrame* apFrame) override;
        CallResult Call(Stack* apStack, void* apLogger, IVirtualMachine* apVm, bool aArg4) override;
        BSFixedString& GetSourceFilename() override;
        bool TranslateIPToLineNumber(uint32_t aTaskletExecutionOffset, uint32_t& aLineNumberOut) override;
        bool GetVarNameForStackIndex(uint32_t aIndex, BSFixedString& aNameOut) override;
        bool CanBeCalledFromTasklets() override;
        void SetCallableFromTasklets(bool aCallable) override;

        virtual bool HasCallback()
        {
            return false;
        }
        virtual bool MarshallAndDispatch(Variable* apBaseVar, IVirtualMachine* apVm, uint32_t aStackID,
                                         Variable* apResult, StackFrame* apStackFrame)
        {
            return false;
        }

        struct Parameters
        {
            struct Entry
            {
                const char* name;
                union
                {
                    Variable::Type type;
                    void* pType;
                };
            };

            Entry* data;
            uint16_t size;
            uint16_t capacity;
        };

        BSFixedString name;
        BSFixedString objectName;
        BSFixedString stateName;
        Variable::Type returnType;
        Parameters parameters;
        bool isStatic;
        bool isCallableFromTask;
        bool isLatent;
        uint8_t pad43;
        uint32_t flags;
        BSFixedString documentation;
    };
    static_assert(sizeof(NativeFunctionBase) == 0x50);

    struct NativeFunction : NativeFunctionBase
    {
        virtual ~NativeFunction()
        {
            TP_THIS_FUNCTION(TNativeFunctionDtor, void, NativeFunction);
            POINTER_FALLOUT4(TNativeFunctionDtor, dtor, 1538707);
            ThisCall(dtor, this);
        }

        NativeFunction(const char* apFunctionName, const char* apClassName, bool aIsStatic, uint32_t aParameterCount)
        {
            TP_THIS_FUNCTION(TNativeFunctionCtor, void, NativeFunction, const char*, const char*, bool, uint32_t);
            POINTER_FALLOUT4(TNativeFunctionCtor, ctor, 861253);
            ThisCall(ctor, this, apFunctionName, apClassName, aIsStatic, aParameterCount);
        }

        bool HasCallback() override
        {
            return pFunction != nullptr;
        }

        void* pFunction;
    };

    struct IsRemotePlayerFunc : NativeFunction
    {
        using FunctionType = bool(Actor* pBase);

        IsRemotePlayerFunc(const char* apFunctionName, const char* apClassName, FunctionType aFunction,
                           Variable::Type aType);

        bool MarshallAndDispatch(Variable* apBaseVar, IVirtualMachine* apVm, uint32_t aStackID, Variable* apResult,
                                 StackFrame* apStackFrame) override;
    };

    struct IsPlayerFunc : NativeFunction
    {
        using FunctionType = bool(Actor* pBase);

        IsPlayerFunc(const char* apFunctionName, const char* apClassName, FunctionType aFunction,
                           Variable::Type aType);

        bool MarshallAndDispatch(Variable* apBaseVar, IVirtualMachine* apVm, uint32_t aStackID, Variable* apResult,
                                 StackFrame* apStackFrame) override;
    };
};

template <> void BSScript::Variable::Set(int32_t aValue) noexcept;
template <> void BSScript::Variable::Set(float aValue) noexcept;
template <> void BSScript::Variable::Set(bool aValue) noexcept;
template <> void BSScript::Variable::Set(const char* acpValue) noexcept;
