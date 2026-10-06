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
