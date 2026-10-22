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
#include <Messages/TradeInventorySyncRequest.h>
#include <Decoding.h>

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

TEST_CASE("Applied crafted poisons identify by recipe, not temporary ID", "[trading][alchemy]")
{
    Inventory::Entry sword;
    sword.BaseId.BaseId = 0x12EB7;
    sword.Count = 1;
    sword.ExtraPoisonId.ModId = 0xFFFFFFFF;
    sword.ExtraPoisonId.BaseId = 0x900;
    sword.ExtraPoisonCount = 3;
    sword.PoisonData = CraftedPotion().Potion;
    sword.PoisonData.IsPoison = true;
    REQUIRE(sword.PoisonData.IsValid());

    auto recreated = sword;
    recreated.ExtraPoisonId.BaseId = 0xABC;
    REQUIRE(SameTradeItem(sword, recreated));
    recreated.PoisonData.Effects[0].Magnitude += 1;
    REQUIRE_FALSE(SameTradeItem(sword, recreated));

    TiltedPhoques::Buffer buffer(1024);
    TiltedPhoques::Buffer::Writer writer(&buffer);
    sword.Serialize(writer);
    TiltedPhoques::Buffer::Reader reader(&buffer);
    Inventory::Entry decoded;
    decoded.Deserialize(reader);
    REQUIRE(decoded == sword);
    REQUIRE(decoded.PoisonData == sword.PoisonData);
}

TEST_CASE("Accepted items must be an in-order subset of the incoming offer", "[trading][alchemy]")
{
    auto potion = CraftedPotion();
    REQUIRE(ValidateAcceptedItems({potion}, {}));
    REQUIRE(ValidateAcceptedItems({potion}, {potion}));
    auto stale = potion;
    stale.Count += 1;
    REQUIRE_FALSE(ValidateAcceptedItems({potion}, {stale}));
    stale = potion;
    stale.Potion.Effects[0].Magnitude += 1;
    REQUIRE_FALSE(ValidateAcceptedItems({potion}, {stale}));
    REQUIRE_FALSE(ValidateAcceptedItems({}, {potion}));
    REQUIRE_FALSE(ValidateAcceptedItems({potion}, {potion, potion}));
    REQUIRE(ValidateAcceptedItems({}, {}));
}

TEST_CASE("Only items each recipient accepts change hands", "[trading][alchemy]")
{
    // A offers a modded-effect potion and a vanilla-effect poison; B offers a vanilla weapon and modded armor.
    auto moddedPotion = CraftedPotion(1);
    moddedPotion.Count = 1;
    moddedPotion.Potion.Effects[0].EffectId.ModId = 7;
    auto poison = CraftedPotion(2);
    poison.Count = 1;
    poison.Potion.IsPoison = true;
    Inventory::Entry weapon;
    weapon.BaseId.BaseId = 0x12EB7;
    weapon.Count = 1;
    weapon.ExtraEnchantId.ModId = 0xFFFFFFFF;
    weapon.ExtraEnchantId.BaseId = 0x800;
    weapon.EnchantData.IsWeapon = true;
    weapon.EnchantData.Effects.push_back(poison.Potion.Effects[0]);
    Inventory::Entry armor;
    armor.BaseId.ModId = 9;
    armor.BaseId.BaseId = 0x801;
    armor.Count = 1;

    Inventory left;
    left.Entries = {moddedPotion, poison};
    Inventory right;
    right.Entries = {weapon, armor};
    const Vector<Inventory::Entry> leftOffer{moddedPotion, poison};
    const Vector<Inventory::Entry> rightOffer{weapon, armor};
    const Vector<Inventory::Entry> acceptedByB{poison};
    const Vector<Inventory::Entry> acceptedByA{weapon};
    REQUIRE(ValidateAcceptedItems(leftOffer, acceptedByB));
    REQUIRE(ValidateAcceptedItems(rightOffer, acceptedByA));
    REQUIRE(MatchAcceptedItems(leftOffer, acceptedByB) == Vector<bool>{false, true});
    REQUIRE(MatchAcceptedItems(rightOffer, acceptedByA) == Vector<bool>{true, false});

    Inventory leftResult;
    Inventory rightResult;
    REQUIRE(PrepareTradeExchange(left, right, acceptedByB, acceptedByA, leftResult, rightResult));
    REQUIRE(leftResult.Entries.size() == 2);
    REQUIRE(SameTradeItem(leftResult.Entries[0], moddedPotion));
    REQUIRE(SameTradeItem(leftResult.Entries[1], weapon));
    REQUIRE(rightResult.Entries.size() == 2);
    REQUIRE(SameTradeItem(rightResult.Entries[0], armor));
    REQUIRE(SameTradeItem(rightResult.Entries[1], poison));
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
        request.AcceptedItems.push_back(potion);
        TiltedPhoques::Buffer::Writer writer(&buffer);
        request.Serialize(writer);
        TiltedPhoques::Buffer::Reader reader(&buffer);
        auto message = ClientMessageFactory{}.Extract(reader);
        REQUIRE(message);
        const auto* decoded = static_cast<TradeSetReadyRequest*>(message.get());
        REQUIRE(decoded->Ready);
        REQUIRE(decoded->AcceptedItems == request.AcceptedItems);
        REQUIRE(decoded->AcceptedItems[0].Potion == potion.Potion);
    }
    SECTION("Full inventory state")
    {
        NotifyTradeState state;
        state.PartnerItems.push_back(potion);
        state.SelfInventory.push_back(potion);
        state.SelfAcceptedItems.push_back(potion);
        TiltedPhoques::Buffer::Writer writer(&buffer);
        state.Serialize(writer);
        TiltedPhoques::Buffer::Reader reader(&buffer);
        auto message = ServerMessageFactory{}.Extract(reader);
        REQUIRE(message);
        const auto* decoded = static_cast<NotifyTradeState*>(message.get());
        REQUIRE(decoded->PartnerItems[0].Potion == potion.Potion);
        REQUIRE(decoded->SelfInventory[0].Potion == potion.Potion);
        REQUIRE(decoded->SelfAcceptedItems.size() == 1);
        REQUIRE(decoded->SelfAcceptedItems[0].Potion == potion.Potion);
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

TEST_CASE("Inventory sync keeps only offered items the live inventory still covers", "[trading]")
{
    Inventory::Entry sword;
    sword.BaseId.BaseId = 0x12EB7;
    sword.Count = 1;
    auto enchanted = sword;
    enchanted.ExtraEnchantId.ModId = 0xFFFFFFFF;
    enchanted.ExtraEnchantId.BaseId = 0x800;
    enchanted.EnchantData.IsWeapon = true;
    enchanted.EnchantData.Effects.push_back(CraftedPotion().Potion.Effects[0]);
    auto potion = CraftedPotion();

    // The server copy still had the plain sword; enchanting changed it in place.
    Inventory live;
    live.Entries = {enchanted, potion};
    const auto kept = KeepCoveredTradeOffer(live, {sword, potion, enchanted});
    REQUIRE(kept.size() == 2);
    REQUIRE(SameTradeItem(kept[0], potion));
    REQUIRE(SameTradeItem(kept[1], enchanted));

    auto twoPotions = potion;
    twoPotions.Count = potion.Count;
    REQUIRE(KeepCoveredTradeOffer(live, {twoPotions, twoPotions}).size() == 1);

    TiltedPhoques::Buffer buffer(4096);
    TradeInventorySyncRequest request;
    request.Entries = live.Entries;
    TiltedPhoques::Buffer::Writer writer(&buffer);
    request.Serialize(writer);
    TiltedPhoques::Buffer::Reader reader(&buffer);
    auto message = ClientMessageFactory{}.Extract(reader);
    REQUIRE(message);
    REQUIRE(message->GetOpcode() == kTradeInventorySyncRequest);
    const auto* decoded = static_cast<TradeInventorySyncRequest*>(message.get());
    REQUIRE(decoded->Entries == request.Entries);
}

namespace
{
Inventory::Entry FullEntry()
{
    auto item = CraftedPotion(7);
    item.ExtraEnchantId.BaseId = 0x800;
    item.EnchantData.IsWeapon = true;
    item.EnchantData.Effects = item.Potion.Effects;
    item.ExtraPoisonId.ModId = 0xFFFFFFFF;
    item.ExtraPoisonId.BaseId = 0x801;
    item.ExtraPoisonCount = 3;
    item.PoisonData = item.Potion;
    item.PoisonData.IsPoison = true;
    return item;
}

// A copy of the first aSize bytes, so the reader runs out where a truncated packet would.
TiltedPhoques::Buffer Prefix(const TiltedPhoques::Buffer& acBuffer, size_t aSize)
{
    return TiltedPhoques::Buffer(acBuffer.GetData(), aSize);
}
} // namespace

TEST_CASE("Inventory entries round trip with every effect list", "[trading][encoding]")
{
    const auto item = FullEntry();
    Inventory::Entry plain;
    plain.BaseId.BaseId = 0xF;
    plain.Count = 1;

    TiltedPhoques::Buffer buffer(4096);
    TradeOfferUpdateRequest request;
    request.Items = {item, plain};
    TiltedPhoques::Buffer::Writer writer(&buffer);
    request.Serialize(writer);

    TiltedPhoques::Buffer exact = Prefix(buffer, writer.Size());
    TiltedPhoques::Buffer::Reader reader(&exact);
    auto message = ClientMessageFactory{}.Extract(reader);
    REQUIRE(message);
    const auto* decoded = static_cast<TradeOfferUpdateRequest*>(message.get());
    REQUIRE(decoded->Items.size() == 2);
    REQUIRE(SameTradeItem(decoded->Items[0], item));
    REQUIRE(decoded->Items[0].Potion == item.Potion);
    REQUIRE(decoded->Items[0].PoisonData == item.PoisonData);
    REQUIRE(decoded->Items[0].ExtraPoisonCount == item.ExtraPoisonCount);
    REQUIRE(decoded->Items[1] == plain);
    REQUIRE(decoded->Items[1].Potion.Effects.empty());
    REQUIRE_FALSE(decoded->Items[1].Potion.IsPoison);
    REQUIRE(decoded->Items[1].PoisonData.Effects.empty());
}

TEST_CASE("Truncated trade packets are rejected", "[trading][encoding]")
{
    TiltedPhoques::Buffer buffer(1 << 16);
    TradeInventorySyncRequest request;
    for (uint32_t i = 0; i < 50; ++i)
    {
        auto item = FullEntry();
        item.BaseId.BaseId = 0x1000 + i;
        request.Entries.push_back(item);
    }
    TiltedPhoques::Buffer::Writer writer(&buffer);
    request.Serialize(writer);
    const size_t cSize = writer.Size();

    // Cut into the first entry, halfway, and a few entries short.
    for (size_t size : {size_t{3}, cSize / 2, cSize - 3 * (cSize / 50)})
    {
        TiltedPhoques::Buffer truncated = Prefix(buffer, size);
        TiltedPhoques::Buffer::Reader reader(&truncated);
        REQUIRE_FALSE(ClientMessageFactory{}.Extract(reader));
    }

    TiltedPhoques::Buffer exact = Prefix(buffer, cSize);
    TiltedPhoques::Buffer::Reader reader(&exact);
    auto message = ClientMessageFactory{}.Extract(reader);
    REQUIRE(message);
    REQUIRE(static_cast<TradeInventorySyncRequest*>(message.get())->Entries.size() == 50);
}

TEST_CASE("Absurd list lengths are rejected without allocating", "[trading][encoding]")
{
    TiltedPhoques::Buffer buffer(4096);
    constexpr uint64_t cHuge = uint64_t{1} << 62;

    SECTION("Client trade lists")
    {
        for (ClientOpcode opcode : {kTradeOfferUpdateRequest, kTradeInventorySyncRequest, kTradeSetReadyRequest})
        {
            TiltedPhoques::Buffer::Writer writer(&buffer);
            writer.WriteBits(opcode, sizeof(ClientOpcode) * 8);
            if (opcode == kTradeSetReadyRequest)
                TiltedPhoques::Serialization::WriteBool(writer, true);
            TiltedPhoques::Serialization::WriteVarInt(writer, cHuge);
            TiltedPhoques::Buffer::Reader reader(&buffer);
            REQUIRE_FALSE(ClientMessageFactory{}.Extract(reader));
        }
    }
    SECTION("Server trade state")
    {
        TiltedPhoques::Buffer::Writer writer(&buffer);
        writer.WriteBits(kNotifyTradeState, sizeof(ServerOpcode) * 8);
        TiltedPhoques::Serialization::WriteVarInt(writer, 42);
        TiltedPhoques::Serialization::WriteBool(writer, false);
        TiltedPhoques::Serialization::WriteBool(writer, false);
        TiltedPhoques::Serialization::WriteVarInt(writer, 0);
        TiltedPhoques::Serialization::WriteVarInt(writer, 0);
        TiltedPhoques::Serialization::WriteVarInt(writer, 0);
        TiltedPhoques::Serialization::WriteVarInt(writer, cHuge);
        TiltedPhoques::Buffer::Reader reader(&buffer);
        REQUIRE_FALSE(ServerMessageFactory{}.Extract(reader));
    }
    SECTION("Effect lists over the limit")
    {
        auto item = FullEntry();
        SECTION("Potion") { item.Potion.Effects.resize(Inventory::kMaxEffects + 1, item.Potion.Effects[0]); }
        SECTION("Poison") { item.PoisonData.Effects.resize(Inventory::kMaxEffects + 1, item.PoisonData.Effects[0]); }
        SECTION("Enchantment") { item.EnchantData.Effects.resize(Inventory::kMaxEffects + 1, item.EnchantData.Effects[0]); }

        NotifyTradeState state;
        state.PartnerItems.push_back(item);
        TiltedPhoques::Buffer::Writer writer(&buffer);
        state.Serialize(writer);
        TiltedPhoques::Buffer::Reader reader(&buffer);
        REQUIRE_FALSE(ServerMessageFactory{}.Extract(reader));
    }
    SECTION("Effect list at the limit")
    {
        auto item = FullEntry();
        item.Potion.Effects.resize(Inventory::kMaxEffects, item.Potion.Effects[0]);
        NotifyTradeState state;
        state.PartnerItems.push_back(item);
        TiltedPhoques::Buffer::Writer writer(&buffer);
        state.Serialize(writer);
        TiltedPhoques::Buffer::Reader reader(&buffer);
        auto message = ServerMessageFactory{}.Extract(reader);
        REQUIRE(message);
        REQUIRE(static_cast<NotifyTradeState*>(message.get())->PartnerItems[0].Potion == item.Potion);
    }
    SECTION("Effect count past the end of the data")
    {
        // A present potion recipe that claims five effects and stops.
        TiltedPhoques::Buffer::Writer writer(&buffer);
        TiltedPhoques::Serialization::WriteBool(writer, true);
        TiltedPhoques::Serialization::WriteBool(writer, false);
        TiltedPhoques::Serialization::WriteVarInt(writer, 5);
        TiltedPhoques::Buffer potion = Prefix(buffer, writer.Size());
        TiltedPhoques::Buffer::Reader reader(&potion);
        Inventory::PotionData decoded;
        decoded.Deserialize(reader);
        REQUIRE(Decoding::IsMalformed(reader));
        REQUIRE(decoded.Effects.empty());
    }
    SECTION("Inventories")
    {
        TiltedPhoques::Buffer::Writer writer(&buffer);
        TiltedPhoques::Serialization::WriteVarInt(writer, cHuge);
        TiltedPhoques::Buffer::Reader reader(&buffer);
        Inventory inventory;
        inventory.Deserialize(reader);
        REQUIRE(Decoding::IsMalformed(reader));
        REQUIRE(inventory.Entries.empty());
    }
}
