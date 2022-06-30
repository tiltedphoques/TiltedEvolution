#pragma once

#include <ExtraData/ExtraDataList.h>
#include <Forms/TESBoundObject.h>

struct BGSInventoryItem
{
    struct Stack : BSIntrusiveRefCounted
    {
        // Seems that most of these flags aren't used.
        // INV_SLOT_INDEX_1 seems to mean not equipped.
        enum class Flag : int32_t
        {
            INV_SLOT_INDEX_1 = 0x0,
            INV_SLOT_INDEX_2 = 0x1,
            INV_SLOT_INDEX_3 = 0x2,
            INV_EQUIP_STATE_LOCKED = 0x3,
            INV_SHOULD_EQUIP = 0x4,
            INV_TEMPORARY = 0x5,
            INV_SLOT_MASK = 0x7,
        };

        virtual ~Stack();

        Stack* spNextStack;
        ExtraDataList* spExtra;
        uint32_t uiCount;
        Flag usFlags;

        // TODO(cosideci): impl range based loop
    #if 0
        struct Iterator 
        {
            Iterator(Stack* apEntry) : m_pEntry(apEntry) {}
            Iterator operator++() { m_pEntry = m_pEntry->spNextStack; return *this; }
            bool operator!=(const Iterator& acRhs) const { return m_pEntry != acRhs.m_pEntry; }
            T* operator*() const { return m_pEntry->data; }
        private:
            Stack* m_pEntry;
        };

        Iterator begin() 
        {
            return Iterator(&entry);
        }

        Iterator end()
        {
            return Iterator(nullptr);
        }
    #endif
    };

    TESBoundObject* pObject;
    Stack* spStackData;
};

static_assert(sizeof(BGSInventoryItem) == 0x10);
