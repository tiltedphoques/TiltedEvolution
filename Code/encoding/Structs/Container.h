#pragma once

#include <TiltedCore/Stl.hpp>
#include <Structs/GameId.h>

using TiltedPhoques::Buffer;
using TiltedPhoques::String;
using TiltedPhoques::Vector;

struct Container
{
    struct EffectItem
    {
        float Magnitude{};
        int32_t Area{};
        int32_t Duration{};
        float RawCost{};
        GameId EffectId{};
    };

    struct EnchantmentData
    {
        bool IsWeapon{};
        Vector<EffectItem> Effects{};

        /*
        int32_t CostOverride{};
        int32_t Flags{};
        int32_t CastingType{};
        int32_t ChargeOverride{};
        int32_t Delivery{};
        int32_t SpellType{};
        float ChargeTime{};
        // TODO: try with IDs for base and worn restrictions first
        // place asserts to see if these are ever temporary
        // should probably support it anyway though
        GameId BaseEnchantmentId{};
        GameId WornRestrictionsId{};
        //Vector<GameId> WornRestrictions{};
        */
    };

    struct Entry
    {
        GameId BaseId{};
        int32_t Count{};

        // TODO: refactor extra data stuff
        // these are the extra data items seemingly relevant to container items
        float ExtraCharge{};

        GameId ExtraEnchantId{};
        uint16_t ExtraEnchantCharge{};
        bool ExtraEnchantRemoveUnequip{};
        EnchantmentData EnchantData{};

        float ExtraHealth{};

        GameId ExtraPoisonId{};
        uint32_t ExtraPoisonCount{};

        int32_t ExtraSoulLevel{};

        String ExtraTextDisplayName{};

        bool ExtraWorn{};
        bool ExtraWornLeft{};

        bool operator==(const Entry& acRhs) const noexcept;
        bool operator!=(const Entry& acRhs) const noexcept;

        void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
        void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

        bool ContainsExtraData() const noexcept
        {
            return !IsExtraDataEquals(Entry{});
        }

        bool CanBeMerged(const Entry& acRhs) const noexcept
        {
            return BaseId == acRhs.BaseId && IsExtraDataEquals(acRhs);
        }

        bool IsExtraDataEquals(const Entry& acRhs) const noexcept
        {
            return ExtraCharge == acRhs.ExtraCharge &&
                   ExtraEnchantId == acRhs.ExtraEnchantId &&
                   ExtraEnchantCharge == acRhs.ExtraEnchantCharge &&
                   ExtraEnchantRemoveUnequip == acRhs.ExtraEnchantRemoveUnequip &&
                   ExtraHealth == acRhs.ExtraHealth &&
                   ExtraPoisonId == acRhs.ExtraPoisonId &&
                   ExtraPoisonCount == acRhs.ExtraPoisonCount &&
                   ExtraSoulLevel == acRhs.ExtraSoulLevel &&
                   //ExtraTextDisplayName == acRhs.ExtraTextDisplayName &&
                   ExtraWorn == acRhs.ExtraWorn &&
                   ExtraWornLeft == acRhs.ExtraWornLeft;
        }
    };

    Container() = default;
    ~Container() = default;

    bool operator==(const Container& acRhs) const noexcept;
    bool operator!=(const Container& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    Vector<Entry> Entries{};
};
