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
};

template <> void BSScript::Variable::Set(int32_t aValue) noexcept;
template <> void BSScript::Variable::Set(float aValue) noexcept;
template <> void BSScript::Variable::Set(bool aValue) noexcept;
template <> void BSScript::Variable::Set(const char* acpValue) noexcept;
