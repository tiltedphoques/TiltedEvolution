#pragma once

struct ActorValueInfo;

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

    virtual float GetNativeValue(const ActorValueInfo& aInfo) const;
    virtual float GetNativePermanentValue(const ActorValueInfo& aInfo) const;
    virtual float GetNativeBaseValue(const ActorValueInfo& aInfo) const;
    virtual void SetNativeBaseValue(const ActorValueInfo& aInfo, float aValue);
    virtual void ModNativeBaseValue(const ActorValueInfo& aInfo, float aValue);
    virtual void ModNativeValue(ForceMode aMode, const ActorValueInfo& aInfo, float aValue);
    virtual float GetNativeModifier(ForceMode aMode, const ActorValueInfo& aInfo) const;
    virtual void RestoreNativeValue(const ActorValueInfo& aInfo, float aValue);
    virtual void SetNativeValue(const ActorValueInfo& aInfo, float aValue);
    virtual bool IsPlayerOwner() const;

    float GetValue(uint32_t aId) const noexcept;
    float GetPermanentValue(uint32_t aId) const noexcept;
    float GetBaseValue(uint32_t aId) const noexcept;
    void SetBaseValue(uint32_t aId, float aValue);
    void ModValue(uint32_t aId, float aValue);
    void ForceCurrent(ForceMode aMode, uint32_t aId, float aValue);
    void SetValue(uint32_t aId, float aValue) noexcept;
};

static_assert(sizeof(ActorValueOwner) == 8);
