#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Allocator.hpp>
#include <TiltedCore/Buffer.hpp>
#include <TiltedCore/Serialization.hpp>

#include <catch2/catch.hpp>

#include <Structs/Inventory.h>

namespace
{
Inventory::Entry EnchantedItem()
{
    Inventory::Entry item{};
    item.BaseId = GameId(1, 0x123);
    item.Count = 1;
    item.ExtraEnchantId = GameId(0xFFFFFFFF, 0x456);
    item.ExtraEnchantCharge = 100;
    item.EnchantData.IsWeapon = true;
    Inventory::EffectItem effect{};
    effect.EffectId = GameId(1, 0x789);
    effect.Magnitude = 10.f;
    effect.Area = 5;
    effect.Duration = 30;
    effect.RawCost = 12.f;
    item.EnchantData.Effects.push_back(effect);
    return item;
}
}

TEST_CASE("Inventory keeps distinct enchantment payloads separate", "[inventory]")
{
    const auto first = EnchantedItem();
    auto second = first;
    SECTION("magnitude") { second.EnchantData.Effects[0].Magnitude += 1.f; }
    SECTION("area") { second.EnchantData.Effects[0].Area += 1; }
    SECTION("duration") { second.EnchantData.Effects[0].Duration += 1; }
    SECTION("cost") { second.EnchantData.Effects[0].RawCost += 1.f; }
    SECTION("effect id") { second.EnchantData.Effects[0].EffectId.BaseId += 1; }
    SECTION("effect count") { second.EnchantData.Effects.push_back(second.EnchantData.Effects[0]); }
    SECTION("enchantment type") { second.EnchantData.IsWeapon = false; }

    REQUIRE(first != second);
    REQUIRE_FALSE(first.CanBeMerged(second));
    Inventory inventory;
    inventory.AddOrRemoveEntry(first);
    inventory.AddOrRemoveEntry(second);
    REQUIRE(inventory.Entries.size() == 2);

    // Removing one item's payload must not consume the other variant.
    second.Count = -1;
    inventory.AddOrRemoveEntry(second);
    REQUIRE(inventory.Entries.size() == 1);
    REQUIRE(inventory.Entries[0] == first);
}

TEST_CASE("Inventory still merges identical enchantments", "[inventory]")
{
    auto item = EnchantedItem();
    Inventory inventory;
    inventory.AddOrRemoveEntry(item);
    inventory.AddOrRemoveEntry(item);
    REQUIRE(inventory.Entries.size() == 1);
    REQUIRE(inventory.Entries[0].Count == 2);
    item.Count = -1;
    inventory.AddOrRemoveEntry(item);
    REQUIRE(inventory.Entries[0].Count == 1);
}
