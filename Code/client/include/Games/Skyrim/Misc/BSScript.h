#pragma once

#if TP_SKYRIM

#include <Games/Skyrim/Misc/BSFixedString.h>
#include <tuple>

struct BSScript
{
    struct Variable
    {
        Variable();
        ~Variable() noexcept;

        void Reset() noexcept;
        void Clear() noexcept;

        template<class T> void Set(T aValue) noexcept
        {
            static_assert(false);
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
        virtual void sub_18();
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
            ((apStatement->values[Is].Set(std::get<Is>(args))), ...);
        }

        Tuple args;
    };
};

template <> void BSScript::Variable::Set(int32_t aValue) noexcept;
template <> void BSScript::Variable::Set(float aValue) noexcept;
template <> void BSScript::Variable::Set(bool aValue) noexcept;
template <> void BSScript::Variable::Set(const char* acpValue) noexcept;

#endif
