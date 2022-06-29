#pragma once

struct BGSInventoryItem
{
    struct Stack : BSIntrusiveRefCounted
    {
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
    };

    TESBoundObject* pObject;
    Stack* spStackData;
};
