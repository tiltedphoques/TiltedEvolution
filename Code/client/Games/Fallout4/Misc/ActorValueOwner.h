#pragma once

#include <Forms/ActorValueInfo.h>

struct ActorValueOwner
{
    virtual ~ActorValueOwner();

    // TODO: refactor these names
    virtual float GetValue(ActorValueInfo* apInfo) const noexcept;
    virtual float GetMaxValue(ActorValueInfo* apInfo) const noexcept;
    virtual float GetBaseValue(ActorValueInfo* apInfo) const noexcept;
    virtual void sub_04();
    virtual void sub_05();
    virtual void ForceCurrent(uint32_t aMode, ActorValueInfo* apInfo, float aValue) noexcept;
    virtual void sub_07();
    virtual void RestoreValue(ActorValueInfo* apInfo, float aAmount) noexcept;
    virtual void SetValue(ActorValueInfo* apInfo, float aValue) noexcept;
};
