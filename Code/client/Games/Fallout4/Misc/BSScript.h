#pragma once

#include <Misc/BSFixedString.h>
#include <Games/Fallout4/TESObjectREFR.h>
#include <Games/Misc/SpinLock.h>

struct BSScript
{
    struct Variable
    {
        Variable() = default;
        ~Variable() noexcept;

        void Reset() noexcept;
        void Clear() noexcept;
        void ConvertToString(char* aBuffer, uint32_t aBufferSize, bool aQuoteStringType, bool aObjectHandleOnly) noexcept;

        template <class T> void Set(T aValue) noexcept
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
            kBoolean,
            kVariable,
            kStruct
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

    struct IObjectHandlePolicy
    {
        virtual ~IObjectHandlePolicy() = 0;

        virtual void sub_01();
        virtual void sub_02();
        virtual void sub_03();
        virtual void sub_04();
        virtual void sub_05();
        virtual void sub_06();
        virtual uint64_t GetHandle(FormType aFormType, TESObjectREFR* apObject);
    };

    struct ObjectTypeInfo
    {
        bool HasVariable(const BSFixedString* aName, bool aCheckValid) noexcept;

        int64_t GetVariableIndex(BSFixedString* aName) noexcept;

        uint8_t pad0[0x10];
        BSFixedString name;
        uint8_t pad18[0x28];
        uint64_t flags1;
        uint8_t pad48[0x8];
        void* data;
    };
    static_assert(sizeof(ObjectTypeInfo) == 0x58);

    struct Object
    {
        Object();
        ~Object() noexcept;

        void IncreaseRef() noexcept;
        void DecreaseRef() noexcept;

        uint8_t pad0[0x8];
        ObjectTypeInfo* typeInfo;
        BSFixedString state;
        uint8_t pad18[0x30 - 0x18];
        Variable variables[1]; // variables are stored like BSFixedString stores characters
    };
    static_assert(sizeof(Object) == 0x40);

    struct Internal
    {
        struct AssociatedScript
        {
            void Cleanup(char aUnkFlag) noexcept;

            uint64_t pointerOrFlags;
        };
    };

    struct AssociatedScriptsTableEntry
    {
        struct EntryValue
        {
            uint32_t size;
            BSScript::Internal::AssociatedScript* externalScript;
        };

        uint64_t key;
        EntryValue value;
        AssociatedScriptsTableEntry* next;
    };
    static_assert(sizeof(AssociatedScriptsTableEntry) == 0x20);

    struct AssociatedScriptsHashMap
    {
        uint8_t pad0[0xC];
        uint32_t size;
        uint8_t pad8[0x8];
        AssociatedScriptsTableEntry* terminator;
        uint8_t pad18[0x8];
        AssociatedScriptsTableEntry* table;

        AssociatedScriptsTableEntry* GetEntry(uint32_t aHash, uint64_t aKey) noexcept
        {
            auto entry = &table[aHash & (size - 1)];
            if (entry)
            {
                if (entry->next)
                {
                    while (entry->key != aKey)
                    {
                        entry = entry->next;
                        if (!entry || entry == terminator)
                            return nullptr;
                    }
                    return entry;
                }
            }
        }
    };
    static_assert(sizeof(AssociatedScriptsHashMap) == 0x30);

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
        virtual bool GetVariable(BSScript::Object** aObject, uint32_t aIndex, BSScript::Variable* aVariable);
        virtual void sub_26();
        virtual void sub_27();
        virtual void sub_28();
        virtual void sub_29();
        virtual void sub_2A();
        virtual void sub_2B();
        virtual void sub_2C();
        virtual void SendEvent(uint64_t aId, const BSFixedString& acEventName,
                               std::function<bool(Statement*)>& aFunctor) const noexcept;
        virtual void sub_2E();
        virtual void sub_2F();
        virtual void sub_30();
        virtual void sub_31();
        virtual void sub_32();
        virtual IObjectHandlePolicy* GetObjectHandlePolicy();

        uint8_t pad0[0xBDF0];
        SpinLock scriptsLock;
        AssociatedScriptsHashMap scriptsMap;
    };

    static void GetObjects(Vector<BSScript::Object*>& aObjects, TESObjectREFR* aObjectRefr) noexcept;
};

template <> void BSScript::Variable::Set(int32_t aValue) noexcept;
template <> void BSScript::Variable::Set(float aValue) noexcept;
template <> void BSScript::Variable::Set(bool aValue) noexcept;
template <> void BSScript::Variable::Set(const char* acpValue) noexcept;
