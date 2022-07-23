#pragma once

#include <Misc/BSFixedString.h>
#include <tuple>

struct BSScript
{
    struct Object
    {
        uint64_t GetHandle();
    };

    struct Variable
    {
        Variable();
        ~Variable() noexcept;

        void Reset() noexcept;
        void Clear() noexcept;
        Object* GetObject() const noexcept;

        template<class T> void Set(T aValue) noexcept
        {
            //static_assert(false);
        }

        enum Type : uint64_t
        {
            kEmpty,
            kObject,
            kString,
            kInteger,
            kFloat,
            kBoolean,

            kMax = 16
        };

        union Data 
        {
            int32_t i;
            const char* s;
            float f;
            bool b;
            Object* pObj;
        };

        Type type;
        Data data;
    };

    struct IFunction : BSIntrusiveRefCounted
    {
        virtual ~IFunction();
        virtual void sub_01();
        virtual void sub_02();
        virtual void sub_03();
        virtual void sub_04();
        virtual void sub_05();
        virtual void sub_06();
        virtual void sub_07();
        virtual void sub_08();
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
    };

    struct NativeFunctionBase : IFunction
    {
        virtual void sub_15();
        virtual void sub_16();

        struct Parameters
        {
            struct Entry
            {
                const char* name;
                Variable::Type type;
            };

            Entry* data;
            uint16_t size;
            uint16_t capacity;
        };

        BSFixedString name;
        BSFixedString objectName;
        BSFixedString stateName;
        Variable::Type type;
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
        void* callback;
    };

    struct IFunctionArguments
    {
        struct Statement
        {
            void SetSize(uint32_t aCount) noexcept;

            uint8_t pad0[8];
            Variable* vars;
        };

        virtual ~IFunctionArguments()
        {
        }

        virtual void Prepare(Statement* apUnk) noexcept = 0;
    };

    struct IObjectHandlePolicy
    {
        static IObjectHandlePolicy* Get() noexcept;

        virtual ~IObjectHandlePolicy();
        virtual bool HandleIsType(uint32_t aType, uint64_t aHandle);
        virtual bool IsHandleObjectAvailable(uint64_t aHandle);
        virtual void sub_03();
        virtual void sub_04();
        virtual void sub_05();
        virtual void sub_06();
        virtual void sub_07();
        virtual void* GetObjectForHandle(uint32_t aType, uint64_t aHandle);

        template <class T>
        T* GetObjectForHandle(uint64_t aHandle)
        {
            return (T*)(BSScript::IObjectHandlePolicy::Get()->GetObjectForHandle((uint32_t)T::Type, aHandle));
        }
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
        virtual void sub_08();
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
        virtual void BindNativeMethod(IFunction* apFunction);
        virtual void sub_19();
        virtual void sub_1A();
        virtual void sub_1B();
        virtual void sub_1C();
        virtual void sub_1D();
        virtual void sub_1E();
        virtual void sub_1F();
        virtual void sub_20();
        virtual void sub_21();
        virtual void sub_22();
        virtual void sub_23();
        virtual void SendEvent(uint64_t aId, const BSFixedString& acEventName, IFunctionArguments* apArgs) const noexcept;
        virtual void sub_25();
        virtual void sub_26();
        virtual void sub_27();
        virtual void sub_28();
        virtual void sub_29();
        virtual void sub_2A();
        virtual void sub_2B();
        virtual void sub_2C();
        virtual IObjectHandlePolicy* GetObjectHandlePolicy();
    };

    template <class... T> struct EventArguments : IFunctionArguments
    {
        using Tuple = std::tuple<EventArguments...>;

        EventArguments(T... args) : args(std::forward<T>(args)...)
        {
        }

        virtual ~EventArguments(){};

        virtual void Prepare(IFunctionArguments::Statement* apStatement) noexcept
        {
            apStatement->SetSize(std::tuple_size_v<std::remove_reference_t<Tuple>>);

            PrepareImplementation(apStatement,
                                  std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
        }

    private:

        template <std::size_t... Is>
        void PrepareImplementation(IFunctionArguments::Statement* apStatement,
                                   std::index_sequence<Is...>) noexcept
        {
            ((apStatement->vars[Is].Set(std::get<Is>(args))), ...);
        }

        Tuple args;
    };
};

template <> void BSScript::Variable::Set(int32_t aValue) noexcept;
template <> void BSScript::Variable::Set(float aValue) noexcept;
template <> void BSScript::Variable::Set(bool aValue) noexcept;
template <> void BSScript::Variable::Set(const char* acpValue) noexcept;
