#pragma once

#include "MagicEquipment.h"

using TiltedPhoques::Buffer;
using TiltedPhoques::String;
using TiltedPhoques::Vector;

struct Inventory
{
    struct EffectItem
    {
        float Magnitude{};
        int32_t Area{};
        int32_t Duration{};
        float RawCost{};
        GameId EffectId{};
        bool operator==(const EffectItem&) const noexcept = default;

        void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
        void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;
    };

    struct EnchantmentData
    {
        bool IsWeapon{};
        Vector<EffectItem> Effects{};
    };

    struct PotionData
    {
        bool IsPoison{};
        // Engine order is kept so recreation matches the original potion; comparison ignores order.
        Vector<EffectItem> Effects{};
        bool operator==(const PotionData& acRhs) const noexcept;
        bool IsValid() const noexcept;
    };

    struct Entry
    {
        GameId BaseId{};
        int32_t Count{};

        float ExtraCharge{};

        GameId ExtraEnchantId{};
        uint16_t ExtraEnchantCharge{};
        EnchantmentData EnchantData{};
        PotionData Potion{};

        float ExtraHealth{};

        GameId ExtraPoisonId{};
        uint32_t ExtraPoisonCount{};
        // Recipe of an applied player-crafted poison, whose temporary ID is only valid for its owner.
        PotionData PoisonData{};

        int32_t ExtraSoulLevel{};

        bool ExtraEnchantRemoveUnequip{};
        bool ExtraWorn{};
        bool ExtraWornLeft{};
        bool IsQuestItem{};

        bool operator==(const Entry& acRhs) const noexcept;
        bool operator!=(const Entry& acRhs) const noexcept;

        void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
        void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

        bool ContainsExtraData() const noexcept { return !IsExtraDataEquals(Entry{}); }

        // Crafted potion identity is its recipe, not another client's temporary form ID.
        bool SameBase(const Entry& acRhs) const noexcept
        {
            if (!Potion.Effects.empty() || !acRhs.Potion.Effects.empty())
                return BaseId.ModId == 0xFFFFFFFF && acRhs.BaseId.ModId == 0xFFFFFFFF && Potion == acRhs.Potion;
            return BaseId == acRhs.BaseId;
        }
        bool CanBeMerged(const Entry& acRhs) const noexcept { return SameBase(acRhs) && IsExtraDataEquals(acRhs); }

        bool IsExtraDataEquals(const Entry& acRhs) const noexcept
        {
            // TODO: the whole server side state thing is very flawed
            // since many of these things can and will change, like poison id or charge
            // or the fact that the enchant id can be temp
            return ExtraCharge == acRhs.ExtraCharge && ExtraEnchantId == acRhs.ExtraEnchantId && ExtraEnchantCharge == acRhs.ExtraEnchantCharge && ExtraEnchantRemoveUnequip == acRhs.ExtraEnchantRemoveUnequip && ExtraHealth == acRhs.ExtraHealth && IsSamePoison(acRhs) &&
                   ExtraPoisonCount == acRhs.ExtraPoisonCount && ExtraSoulLevel == acRhs.ExtraSoulLevel && ExtraWorn == acRhs.ExtraWorn && ExtraWornLeft == acRhs.ExtraWornLeft && IsQuestItem == acRhs.IsQuestItem;
        }

        bool IsSamePoison(const Entry& acRhs) const noexcept
        {
            if (!PoisonData.Effects.empty() || !acRhs.PoisonData.Effects.empty())
                return ExtraPoisonId.ModId == 0xFFFFFFFF && acRhs.ExtraPoisonId.ModId == 0xFFFFFFFF && PoisonData == acRhs.PoisonData;
            return ExtraPoisonId == acRhs.ExtraPoisonId;
        }

        bool IsWorn() const noexcept { return ExtraWorn || ExtraWornLeft; }
    };

    bool operator==(const Inventory& acRhs) const noexcept;
    bool operator!=(const Inventory& acRhs) const noexcept;

    void Serialize(TiltedPhoques::Buffer::Writer& aWriter) const noexcept;
    void Deserialize(TiltedPhoques::Buffer::Reader& aReader) noexcept;

    std::optional<Entry> GetEntryById(GameId& aItemId) const noexcept;
    int32_t GetEntryCountById(GameId& aItemId) const noexcept;

    void RemoveByFilter(std::function<bool(const Entry&)> aFilter) noexcept;
    void AddOrRemoveEntry(const Entry& acEntry) noexcept;
    void UpdateEquipment(const Inventory& acNewInventory) noexcept;
    bool ContainsQuestItems() const noexcept;

    Vector<Entry> Entries{};
    MagicEquipment CurrentMagicEquipment{};
};
