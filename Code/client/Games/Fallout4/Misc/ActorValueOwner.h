#pragma once

#include <Forms/ActorValueInfo.h>

struct ActorValueOwner
{
    virtual ~ActorValueOwner();

    virtual float GetValue(ActorValueInfo* apInfo) const noexcept;
    virtual float GetValuePercentage(ActorValueInfo* apInfo) const noexcept;
    virtual float GetBaseValue(ActorValueInfo* apInfo) const noexcept;
    virtual void sub_04();
    virtual void sub_05();
    virtual void ModValue(ActorValueInfo* apInfo, float aAmount) noexcept;
    virtual void sub_07();
    virtual void RestoreValue(ActorValueInfo* apInfo, float aAmount) noexcept;
    virtual void SetValue(ActorValueInfo* apInfo, float aValue) noexcept;
};
