#pragma once

struct ActorValueOwner
{
    virtual ~ActorValueOwner();

    virtual float GetValue(uint32_t aId) const noexcept;
    virtual float GetMaxValue(uint32_t aId) const noexcept;
    virtual float GetBaseValue(uint32_t aId);
    virtual void sub_4();
    virtual void sub_5();
    virtual void ForceCurrent(uint32_t aUnk1, uint32_t aId, float aValue); // Pass 0 for aUnk1 in force, 2 in restore
    virtual void SetValue(uint32_t aId, float aValue) noexcept;
    virtual void sub_8();
};
