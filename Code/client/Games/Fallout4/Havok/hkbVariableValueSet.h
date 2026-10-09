#pragma once

template <class T> struct hkbVariableValueSet
{
    virtual ~hkbVariableValueSet();

    uint8_t pad8[0x8]; // 8
    T* data;           // 10
    uint32_t size;     // 18
};

static_assert(offsetof(hkbVariableValueSet<int>, data) == 0x10);
static_assert(offsetof(hkbVariableValueSet<int>, size) == 0x18);
