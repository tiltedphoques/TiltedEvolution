#pragma once

struct ActorValueOwner
{
    enum class ForceMode : uint32_t
    {
        PERMANENT = 0,
        TEMPORARY = 1,
        DAMAGE = 2,
        COUNT = 3,
    };

    virtual ~ActorValueOwner();

    virtual float GetValue(uint32_t aId) const noexcept;
    virtual float GetPermanentValue(uint32_t aId) const noexcept;
    virtual float GetBaseValue(uint32_t aId) const noexcept;
    virtual void SetBaseValue(uint32_t aId);
    virtual void ModValue(uint32_t aId, float aValue);
    virtual void ForceCurrent(ForceMode aMode, uint32_t aId, float aValue);
    virtual void SetValue(uint32_t aId, float aValue) noexcept;
    virtual bool IsPlayerOwner();
};
