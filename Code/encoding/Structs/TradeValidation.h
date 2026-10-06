#pragma once

#include <Structs/Inventory.h>
#include <algorithm>
#include <limits>

inline bool SameTradeItem(const Inventory::Entry& left, const Inventory::Entry& right) noexcept
{
    if (!left.CanBeMerged(right) || left.EnchantData.IsWeapon != right.EnchantData.IsWeapon ||
        left.EnchantData.Effects.size() != right.EnchantData.Effects.size())
        return false;
    for (size_t i = 0; i < left.EnchantData.Effects.size(); ++i)
    {
        const auto& a = left.EnchantData.Effects[i];
        const auto& b = right.EnchantData.Effects[i];
        if (a.EffectId != b.EffectId || a.Magnitude != b.Magnitude || a.Area != b.Area || a.Duration != b.Duration || a.RawCost != b.RawCost)
            return false;
    }
    return true;
}

// Reserve quantities across the entire offer so repeated entries cannot reuse the same items.
inline bool ValidateTradeOffer(const Inventory& acInventory, const Vector<Inventory::Entry>& acItems) noexcept
{
    Vector<int32_t> remaining;
    remaining.reserve(acInventory.Entries.size());
    for (const auto& entry : acInventory.Entries)
        remaining.push_back(std::max(0, entry.Count));

    for (const auto& item : acItems)
    {
        if (item.Count <= 0 || item.IsQuestItem || item.IsWorn())
            return false;

        int32_t needed = item.Count;
        for (size_t i = 0; i < acInventory.Entries.size() && needed > 0; ++i)
        {
            const auto& entry = acInventory.Entries[i];
            if (!SameTradeItem(entry, item))
                continue;

            const int32_t reserved = std::min(needed, remaining[i]);
            remaining[i] -= reserved;
            needed -= reserved;
        }
        if (needed != 0)
            return false;
    }
    return true;
}

// Publish neither inventory until both offers and resulting stack counts are valid.
inline bool PrepareTradeExchange(const Inventory& acLeft, const Inventory& acRight,
    const Vector<Inventory::Entry>& acLeftOffer, const Vector<Inventory::Entry>& acRightOffer,
    Inventory& aLeftResult, Inventory& aRightResult) noexcept
{
    if (!ValidateTradeOffer(acLeft, acLeftOffer) || !ValidateTradeOffer(acRight, acRightOffer))
        return false;
    Inventory left = acLeft;
    Inventory right = acRight;
    auto remove = [](Inventory& inventory, const Vector<Inventory::Entry>& offer)
    {
        for (const auto& item : offer)
        {
            int32_t needed = item.Count;
            for (auto& entry : inventory.Entries)
            {
                if (!SameTradeItem(entry, item) || entry.Count <= 0)
                    continue;
                const int32_t count = std::min(needed, entry.Count);
                entry.Count -= count;
                needed -= count;
                if (!needed)
                    break;
            }
        }
        inventory.Entries.erase(std::remove_if(inventory.Entries.begin(), inventory.Entries.end(),
            [](const auto& entry) { return entry.Count == 0; }), inventory.Entries.end());
    };
    auto add = [](Inventory& inventory, const Vector<Inventory::Entry>& offer)
    {
        for (const auto& item : offer)
        {
            auto it = std::find_if(inventory.Entries.begin(), inventory.Entries.end(),
                [&item](const auto& entry) { return SameTradeItem(entry, item); });
            if (it == inventory.Entries.end())
                inventory.Entries.push_back(item);
            else
            {
                if (it->Count < 0 || it->Count > std::numeric_limits<int32_t>::max() - item.Count)
                    return false;
                it->Count += item.Count;
            }
        }
        return true;
    };
    remove(left, acLeftOffer);
    remove(right, acRightOffer);
    if (!add(left, acRightOffer) || !add(right, acLeftOffer))
        return false;
    aLeftResult = std::move(left);
    aRightResult = std::move(right);
    return true;
}
