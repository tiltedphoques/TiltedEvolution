#pragma once

#include <Components/BaseFormComponent.h>

struct TESForm;

struct TESLeveledList : BaseFormComponent
{
    virtual ~TESLeveledList();

    struct Entry
    {
        TESForm* form;
        void* unkPtr;
        uint16_t minLevel;
        uint16_t maxLevel;
        uint8_t unk14; // Specific to fallout 4
    };

    struct Content
    {
        uint64_t length; // it's set to the size of the allocated array for some reason
        Entry entries[0];
    };

    uint8_t pad8[0x18 - 0x8];
    Content* content;
    uint8_t pad20[0x28 - 0x20];
    bool hasData;
    uint8_t count;
};

static_assert(sizeof(TESLeveledList::Entry) == 0x18);
static_assert(offsetof(TESLeveledList::Entry, unk14) == 0x14);
static_assert(offsetof(TESLeveledList, content) == 0x18);
static_assert(offsetof(TESLeveledList, count) == 0x29);
static_assert(offsetof(TESLeveledList, hasData) == 0x28);
static_assert(sizeof(TESLeveledList) == 0x30);
