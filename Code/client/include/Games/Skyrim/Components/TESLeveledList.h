#pragma once

#include <Components/BaseFormComponent.h>

struct TESForm;

struct TESLeveledList : BaseFormComponent
{
    struct Entry
    {
        TESForm* form;
        uint16_t minLevel;
        uint16_t maxLevel;
        void* unk10;
    };

    struct Content
    {
        uint64_t length; // it's set to the size of the allocated array for some reason
        Entry entries[0];
    };

    Content* content;
    uint8_t pad8[0x12 - 0x10];
    uint8_t count;
    bool hasData;
    uint8_t pad14[0x28 - 0x14];
};

static_assert(sizeof(TESLeveledList::Entry) == 0x18);
static_assert(offsetof(TESLeveledList, content) == 0x8);
static_assert(offsetof(TESLeveledList, count) == 0x12);
static_assert(offsetof(TESLeveledList, hasData) == 0x13);
static_assert(sizeof(TESLeveledList) == 0x28);
