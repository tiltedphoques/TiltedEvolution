#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Buffer.hpp>
#include <TiltedCore/Serialization.hpp>
#include <optional>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <Structs/GameId.h>
#include <Structs/TradeValidation.h>
#include <Messages/ClientMessageFactory.h>
#include <Messages/ServerMessageFactory.h>
#include <catch2/catch.hpp>
#include <limits>

namespace
{
Inventory::Entry CraftedPotion(uint32_t aLocalId = 1)
{
    Inventory::Entry item;
    item.BaseId.ModId = 0xFFFFFFFF;
    item.BaseId.BaseId = aLocalId;
    item.Count = 2;
    Inventory::EffectItem effect;
    effect.EffectId.BaseId = 0x3EB15;
    effect.Magnitude = 37.5f;
    effect.Duration = 12;
    effect.RawCost = 42.0f;
    item.Potion.Effects.push_back(effect);
    return item;
}
}

TEST_CASE("Crafted potion trades identify recipes independently of temporary form IDs", "[trading][alchemy]")
{
    auto source = CraftedPotion(123);
    auto recipient = CraftedPotion(987);
    REQUIRE(source.Potion.IsValid());
    REQUIRE(SameTradeItem(source, recipient));
    Inventory left, right, leftResult, rightResult;
    left.Entries.push_back(source);
    right.Entries.push_back(recipient);
    auto offer = source;
    offer.Count = 1;
    REQUIRE(PrepareTradeExchange(left, right, {offer}, {}, leftResult, rightResult));
    REQUIRE(leftResult.Entries[0].Count == 1);
    REQUIRE(rightResult.Entries.size() == 1);
    REQUIRE(rightResult.Entries[0].Count == 3);
    REQUIRE(rightResult.Entries[0].Potion == source.Potion);
    // Drinking or transferring the recipient's locally recreated ID updates the same server stack.
    recipient.Count = -1;
    rightResult.AddOrRemoveEntry(recipient);
    REQUIRE(rightResult.Entries[0].Count == 2);
    SECTION("Magnitude, duration and type cannot be substituted")
    {
        recipient.Potion.Effects[0].Magnitude += 1;
        REQUIRE_FALSE(SameTradeItem(source, recipient));
        recipient = source;
        recipient.Potion.Effects[0].Duration += 1;
        REQUIRE_FALSE(SameTradeItem(source, recipient));
        recipient = source;
        recipient.Potion.IsPoison = true;
        REQUIRE_FALSE(SameTradeItem(source, recipient));
    }
    SECTION("Unknown temporary forms and invalid effect data cannot enter an offer")
    {
        offer.Potion.Effects.clear();
        REQUIRE(IsExcludedTradeItem(offer));
        offer = source;
        offer.Potion.Effects[0].Magnitude = std::numeric_limits<float>::quiet_NaN();
        REQUIRE(IsExcludedTradeItem(offer));
        offer = source;
        offer.Potion.Effects[0].EffectId.ModId = 0xFFFFFFFF;
        REQUIRE(IsExcludedTradeItem(offer));
    }
    SECTION("Static food remains ordinary form-based inventory")
    {
        Inventory::Entry meal;
        meal.BaseId.BaseId = 0x64B3F;
        meal.Count = 1;
        REQUIRE_FALSE(IsExcludedTradeItem(meal));
        REQUIRE_FALSE(SameTradeItem(meal, source));
    }
}

TEST_CASE("Crafted potion recipes compare independently of effect order", "[trading][alchemy]")
{
    auto first = CraftedPotion();
    first.Potion.Effects.push_back(first.Potion.Effects[0]);
    first.Potion.Effects[1].EffectId.BaseId += 1;
    first.Potion.Effects[1].Magnitude = 5.0f;
    auto second = first;
    std::swap(second.Potion.Effects[0], second.Potion.Effects[1]);
    REQUIRE(first.Potion == second.Potion);
    REQUIRE(SameTradeItem(first, second));

    second.Potion.Effects[0] = second.Potion.Effects[1];
    REQUIRE_FALSE(first.Potion == second.Potion);
}

TEST_CASE("Potion readiness proofs must match the complete incoming offer", "[trading][alchemy]")
{
    auto potion = CraftedPotion();
    REQUIRE_FALSE(ValidatePreparedPotions({potion}, {}));
    REQUIRE(ValidatePreparedPotions({potion}, {potion}));
    auto stale = potion;
    stale.Count += 1;
    REQUIRE_FALSE(ValidatePreparedPotions({potion}, {stale}));
    stale = potion;
    stale.Potion.Effects[0].Magnitude += 1;
    REQUIRE_FALSE(ValidatePreparedPotions({potion}, {stale}));
    REQUIRE_FALSE(ValidatePreparedPotions({}, {potion}));
    REQUIRE_FALSE(ValidatePreparedPotions({potion}, {potion, potion}));
    REQUIRE(ValidatePreparedPotions({}, {}));
}

TEST_CASE("Potion recipes and readiness proofs round trip through packet factories", "[trading][encoding][alchemy]")
{
    TiltedPhoques::Buffer buffer(4096);
    auto potion = CraftedPotion();
    potion.Potion.IsPoison = true;
    potion.Potion.Effects.push_back(potion.Potion.Effects[0]);
    potion.Potion.Effects[1].EffectId.BaseId += 1;
    potion.Potion.Effects[1].Area = 3;
    SECTION("Ready with a prepared multi-effect poison")
    {
        TradeSetReadyRequest request;
        request.Ready = true;
        request.PreparedPotions.push_back(potion);
        TiltedPhoques::Buffer::Writer writer(&buffer);
        request.Serialize(writer);
        TiltedPhoques::Buffer::Reader reader(&buffer);
        auto message = ClientMessageFactory{}.Extract(reader);
        REQUIRE(message);
        const auto* decoded = static_cast<TradeSetReadyRequest*>(message.get());
        REQUIRE(decoded->Ready);
        REQUIRE(decoded->PreparedPotions == request.PreparedPotions);
        REQUIRE(decoded->PreparedPotions[0].Potion == potion.Potion);
    }
    SECTION("Full inventory state")
    {
        NotifyTradeState state;
        state.PartnerItems.push_back(potion);
        state.SelfInventory.push_back(potion);
        TiltedPhoques::Buffer::Writer writer(&buffer);
        state.Serialize(writer);
        TiltedPhoques::Buffer::Reader reader(&buffer);
        auto message = ServerMessageFactory{}.Extract(reader);
        REQUIRE(message);
        const auto* decoded = static_cast<NotifyTradeState*>(message.get());
        REQUIRE(decoded->PartnerItems[0].Potion == potion.Potion);
        REQUIRE(decoded->SelfInventory[0].Potion == potion.Potion);
    }
}

TEST_CASE("Trade offers reserve inventory quantities", "[trading]")
{
    Inventory inventory;
    Inventory::Entry entry;
    entry.BaseId.BaseId = 0xF;
    entry.Count = 10;
    inventory.Entries.push_back(entry);

    entry.Count = 6;
    Vector<Inventory::Entry> offer{entry, entry};
    REQUIRE_FALSE(ValidateTradeOffer(inventory, offer));
    offer[1].Count = 4;
    REQUIRE(ValidateTradeOffer(inventory, offer));
    REQUIRE(inventory.Entries[0].Count == 10);

    SECTION("Quest items cannot be offered")
    {
        offer[0].IsQuestItem = true;
        REQUIRE_FALSE(ValidateTradeOffer(inventory, offer));
    }
    SECTION("Removing a quest flag cannot bypass the canonical inventory check")
    {
        inventory.Entries[0].IsQuestItem = true;
        REQUIRE_FALSE(ValidateTradeOffer(inventory, offer));
    }
    SECTION("The internal unarmed weapon cannot be traded")
    {
        inventory.Entries[0].BaseId.BaseId = 0x1F4;
        offer[0].BaseId.BaseId = offer[1].BaseId.BaseId = 0x1F4;
        REQUIRE_FALSE(ValidateTradeOffer(inventory, offer));
    }
    SECTION("Counts must be positive")
    {
        offer[0].Count = -1;
        REQUIRE_FALSE(ValidateTradeOffer(inventory, offer));
    }
    SECTION("Extra data must match")
    {
        offer[0].ExtraHealth = 2.0f;
        REQUIRE_FALSE(ValidateTradeOffer(inventory, offer));
    }
    SECTION("Custom enchantment effects must match")
    {
        offer[0].EnchantData.Effects.push_back(Inventory::EffectItem{});
        REQUIRE_FALSE(ValidateTradeOffer(inventory, offer));
    }
    SECTION("Equipped items must be unequipped before trading")
    {
        inventory.Entries[0].ExtraWorn = true;
        offer[0].ExtraWorn = true;
        REQUIRE_FALSE(ValidateTradeOffer(inventory, offer));
    }
}

TEST_CASE("Trade exchange stages both inventories before publishing", "[trading]")
{
    Inventory left;
    Inventory right;
    Inventory::Entry gold;
    gold.BaseId.BaseId = 0xF;
    gold.Count = 10;
    left.Entries.push_back(gold);
    Inventory::Entry item;
    item.BaseId.BaseId = 0x123;
    item.Count = 1;
    right.Entries.push_back(item);
    Inventory leftResult = left;
    Inventory rightResult = right;
    Vector<Inventory::Entry> leftOffer{gold};
    Vector<Inventory::Entry> rightOffer{item};

    SECTION("Valid exchange conserves quantities")
    {
        REQUIRE(PrepareTradeExchange(left, right, leftOffer, rightOffer, leftResult, rightResult));
        REQUIRE(leftResult.Entries == right.Entries);
        REQUIRE(rightResult.Entries == left.Entries);
        REQUIRE(left.Entries[0].Count == 10);
        REQUIRE(right.Entries[0].Count == 1);
    }
    SECTION("Invalid second offer publishes neither inventory")
    {
        rightOffer[0].Count = 2;
        REQUIRE_FALSE(PrepareTradeExchange(left, right, leftOffer, rightOffer, leftResult, rightResult));
        REQUIRE(leftResult == left);
        REQUIRE(rightResult == right);
    }
    SECTION("Overflow publishes neither inventory")
    {
        right.Entries.push_back(gold);
        right.Entries.back().Count = std::numeric_limits<int32_t>::max();
        rightResult = right;
        REQUIRE_FALSE(PrepareTradeExchange(left, right, leftOffer, rightOffer, leftResult, rightResult));
        REQUIRE(leftResult == left);
        REQUIRE(rightResult == right);
    }
}

TEST_CASE("Trade packets round trip through message factories", "[trading][encoding]")
{
    TiltedPhoques::Buffer buffer(4096);
    Inventory::Entry item;
    item.BaseId.BaseId = 0xF;
    item.Count = 8;

    SECTION("Offer update")
    {
        TradeOfferUpdateRequest request;
        request.Items.push_back(item);
        TiltedPhoques::Buffer::Writer writer(&buffer);
        request.Serialize(writer);
        TiltedPhoques::Buffer::Reader reader(&buffer);
        auto message = ClientMessageFactory{}.Extract(reader);
        REQUIRE(message);
        REQUIRE(message->GetOpcode() == TradeOfferUpdateRequest::Opcode);
        const auto* decoded = static_cast<TradeOfferUpdateRequest*>(message.get());
        REQUIRE(decoded->Items == request.Items);
    }
    SECTION("Session state and countdown")
    {
        NotifyTradeState state;
        state.PartnerPlayerId = 42;
        state.SelfReady = true;
        state.CountdownMs = 2000;
        state.CountdownTotalMs = 4000;
        state.SelfItems.push_back(item);
        state.SelfInventory.push_back(item);
        TiltedPhoques::Buffer::Writer writer(&buffer);
        state.Serialize(writer);
        TiltedPhoques::Buffer::Reader reader(&buffer);
        auto message = ServerMessageFactory{}.Extract(reader);
        REQUIRE(message);
        REQUIRE(message->GetOpcode() == NotifyTradeState::Opcode);
        const auto* decoded = static_cast<NotifyTradeState*>(message.get());
        REQUIRE(decoded->PartnerPlayerId == state.PartnerPlayerId);
        REQUIRE(decoded->SelfReady);
        REQUIRE(decoded->CountdownMs == state.CountdownMs);
        REQUIRE(decoded->CountdownTotalMs == state.CountdownTotalMs);
        REQUIRE(decoded->SelfItems == state.SelfItems);
        REQUIRE(decoded->SelfInventory == state.SelfInventory);
    }
}
