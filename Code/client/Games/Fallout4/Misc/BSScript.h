#pragma once

#include <Misc/BSFixedString.h>
#include <Games/Fallout4/TESObjectREFR.h>
#include <Games/Misc/SpinLock.h>

namespace BSScript
{
    struct PropertyGroupInfo : BSIntrusiveRefCounted
    {
        BSFixedString sGroupName;
        BSFixedString sDocStrings;
        uint32_t uiUserFlags;
        GameArray<BSFixedString> kPropertyNames;
    };
    static_assert(sizeof(BSScript::PropertyGroupInfo) == 0x38);

    struct IComplexType : BSIntrusiveRefCounted
    {
        virtual ~IComplexType();
        virtual BSScript::TypeInfo::RawType GetRawType();
    };
    static_assert(sizeof(IComplexType) == 0x10);

    struct ObjectTypeInfo : IComplexType
    {
        int64_t GetVariableIndex(BSFixedString* aName) noexcept;

        BSFixedString sName;
        ObjectTypeInfo* pParentTypeInfo;
        BSFixedString sDocString;
        GameArray<PropertyGroupInfo*> kPropertyGroups;
        int32_t eLinkedValid : 2;
        int32_t bConst : 1;
        int32_t uiUserFlagCount : 5;
        int32_t uiVariableCount : 10;
        int32_t uiVariableUserFlagCount : 6;
        int32_t uiInitialValueCount : 10;
        int32_t uiPropertyCount : 10;
        int32_t uiStaticFunctionCount : 9;
        int32_t uiEmptyStateMemberFunctionCount : 11;
        int32_t uiNamedStateCount : 7;
        int32_t uiInitialState : 7;
        void *pData;
    };
    static_assert(sizeof(BSScript::ObjectTypeInfo) == 0x58);

    struct TypeInfo
    {
        enum RawType : int32_t
        {
            NONE = 0x0,                         // kEmpty
            OBJECT = 0x1,                       // kHandle
            STRING = 0x2,                       // kString
            INT = 0x3,                          // kInteger
            FLOAT = 0x4,                        // kFloat
            BOOL = 0x5,                         // kBoolean
            VAR = 0x6,
            STRUCT = 0x7,
            TYPE_MAX = 0x8,
            ARRAY_TYPE_START = 0xA,
            ARRAY_OBJECT = 0xB,
            ARRAY_STRING = 0xC,
            ARRAY_INT = 0xD,
            ARRAY_FLOAT = 0xE,
            ARRAY_BOOL = 0xF,
            ARRAY_VAR = 0x10,
            ARRAY_STRUCT = 0x11,
            ARRAY_TYPE_MAX = 0x12,
            RAWTYPE_FORCE_UINT32_SIZE = -1,
        };

        union
        {
            RawType eRawType;
            IComplexType* pComplexTypeInfo;
        } uData;
    };
    static_assert(sizeof(BSScript::TypeInfo) == 0x8);

    struct Variable
    {
        Variable() = default;
        ~Variable() noexcept;

        void Reset() noexcept;
        void Clear() noexcept;
        void ConvertToString(char* aBuffer, uint32_t aBufferSize, bool aQuoteStringType,
                             bool aObjectHandleOnly) noexcept;

        template<class T> void Set(T aValue) noexcept
        {
            static_assert(false);
        }

        union Data 
        {
            int32_t i;
            const char* s;
            float f;
            bool b;
        };

        TypeInfo VarType;
        Data uiValue;
    };
    static_assert(sizeof(BSScript::Variable) == 0x10);

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

    struct Object
    {
        void IncreaseRef() noexcept;
        void DecreaseRef() noexcept;

        uint32_t bConstructed : 1;
        uint32_t bInitialized : 1;
        uint32_t bValid : 1;
        uint32_t uiRemainingPropsToInit : 29;
        ObjectTypeInfo* pType;
        BSFixedString sCurrentState;
        void* pLockStructure;
        uint64_t hHandle;
        uint32_t uiRefCountAndHandleLock;
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

    // BSTScatterTableEntry
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

    // BSTScatterTable
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

        // TODO: move members outside of interface
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
