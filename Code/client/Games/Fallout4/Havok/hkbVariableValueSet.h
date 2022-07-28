#pragma once

// TODO: this should be a template
struct hkbVariableValueSet
{
    virtual ~hkbVariableValueSet();

    uint8_t pad8[0x8]; // 8
    uint32_t* data; // 10
    uint32_t size; // 18
};

static_assert(offsetof(hkbVariableValueSet, data) == 0x10);
static_assert(offsetof(hkbVariableValueSet, size) == 0x18);
