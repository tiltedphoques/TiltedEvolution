#pragma once

#include <TiltedCore/Stl.hpp>
#include <Structs/GameId.h>

using TiltedPhoques::Buffer;

struct Container
{
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

        float ExtraHealth{};

        GameId ExtraPoisonId{};
        uint32_t ExtraPoisonCount{};

        int32_t ExtraSoulLevel{};

        String ExtraTextDisplayName{};

        bool ExtraWorn{};
        bool ExtraWornLeft{};

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

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;


    Vector<Entry> Entries{};
};
