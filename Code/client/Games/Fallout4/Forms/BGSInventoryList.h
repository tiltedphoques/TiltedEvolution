#pragma once

#include <Events/EventDispatcher.h>
#include "BGSInventoryItem.h"

struct BGSInventoryList : EventDispatcher<BGSInventoryListEvent::Event>
{
    // TODO: maybe use this?
    struct __declspec(align(4)) IsQuestObjectFunctor
    {
        TESBoundObject* pObj;
        uint32_t uiStackID;
        bool bResult;
    };

    GameArray<BGSInventoryItem> DataA;
    float fCachedWeight;
    BSPointerHandle<TESObjectREFR> hOwner;
    BSReadWriteLock RWLock;
};

static_assert(sizeof(BGSInventoryList) == 0x80);
