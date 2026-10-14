#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Allocator.hpp>
#include <TiltedCore/Buffer.hpp>
#include <TiltedCore/Serialization.hpp>

#include <optional>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "StringCache.h"
#include "Messages/StringCacheUpdate.h"

#include <catch2/catch.hpp>

#include <Messages/ClientMessageFactory.h>
#include <Messages/ServerMessageFactory.h>
#include <Structs/Vector2_NetQuantize.h>

#include <TiltedCore/Math.hpp>
#include <TiltedCore/Platform.hpp>

using namespace TiltedPhoques;

TEST_CASE("Encoding factory", "[encoding.factory]")
{
    Buffer buff(1000);

    {
        AuthenticationRequest request;
        request.Token = "TesSt";

        Buffer::Writer writer(&buff);
        request.Serialize(writer);

        Buffer::Reader reader(&buff);

        const ClientMessageFactory factory;
        auto pMessage = factory.Extract(reader);

        REQUIRE(pMessage);
        REQUIRE(pMessage->GetOpcode() == request.GetOpcode());

        auto pRequest = CastUnique<AuthenticationRequest>(std::move(pMessage));
        REQUIRE(pRequest->Token == request.Token);
    }

    {
        PartyAcceptInviteRequest request;
        request.InviterId = 123456;

        Buffer::Writer writer(&buff);
        request.Serialize(writer);

        Buffer::Reader reader(&buff);

        const ClientMessageFactory factory;
        auto pMessage = factory.Extract(reader);

        REQUIRE(pMessage);
        REQUIRE(pMessage->GetOpcode() == request.GetOpcode());

        auto pRequest = CastUnique<PartyAcceptInviteRequest>(std::move(pMessage));
        REQUIRE(pRequest->InviterId == request.InviterId);
    }
}

TEST_CASE("Static structures", "[encoding.static]")
{
    GIVEN("GameId")
    {
        GameId sendObjects, recvObjects;
        sendObjects.ModId = 1456987;
        sendObjects.BaseId = 0x789654;

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendObjects.Serialize(writer);

            Buffer::Reader reader(&buff);
            recvObjects.Deserialize(reader);

            REQUIRE(sendObjects == recvObjects);
        }
    }

    GIVEN("Vector3_NetQuantize")
    {
        Vector3_NetQuantize sendObjects, recvObjects;
        sendObjects.x = 142.56f;
        sendObjects.y = 45687.7f;
        sendObjects.z = -142.56f;

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendObjects.Serialize(writer);

            Buffer::Reader reader(&buff);
            recvObjects.Deserialize(reader);

            REQUIRE(sendObjects == recvObjects);
        }
    }

    GIVEN("Vector2_NetQuantize")
    {
        Vector2_NetQuantize sendObjects, recvObjects;
        sendObjects.x = 1000.89f;
        sendObjects.y = -485632.75f;

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendObjects.Serialize(writer);

            Buffer::Reader reader(&buff);
            recvObjects.Deserialize(reader);

            REQUIRE(sendObjects == recvObjects);
        }
    }

    GIVEN("Rotator2_NetQuantize")
    {
        Rotator2_NetQuantize sendObjects, recvObjects;
        sendObjects.x = 1.89f;
        sendObjects.y = TiltedPhoques::Pi * 2.0f;

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendObjects.Serialize(writer);

            Buffer::Reader reader(&buff);
            recvObjects.Deserialize(reader);

            REQUIRE(sendObjects == recvObjects);
        }
    }

    GIVEN("Rotator2_NetQuantize needing wrap")
    {
        // This test is a bit dangerous as floating errors can lead to sendObjects != recvObjects but the difference is minuscule so we don't care abut such cases
        Rotator2_NetQuantize sendObjects, recvObjects;
        sendObjects.x = -1.87f;
        sendObjects.y = static_cast<float>(TiltedPhoques::Pi) * 18.0f + 3.6f;

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendObjects.Serialize(writer);

            Buffer::Reader reader(&buff);
            recvObjects.Deserialize(reader);

            REQUIRE(sendObjects == recvObjects);
        }
    }
}

TEST_CASE("Differential structures", "[encoding.differential]")
{
    GIVEN("Full ActionEvent")
    {
        ActionEvent sendAction, recvAction;

        sendAction.ActionId = 42;
        sendAction.State1 = 6547;
        sendAction.Tick = 48;
        sendAction.ActorId = 12345678;
        sendAction.EventName = "test";
        sendAction.IdleId = 87964;
        sendAction.State2 = 8963;
        sendAction.TargetEventName = "toast";
        sendAction.TargetId = 963741;
        sendAction.Type = 4;

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendAction.GenerateDifferential(recvAction, writer);

            Buffer::Reader reader(&buff);
            recvAction.ApplyDifferential(reader);

            REQUIRE(sendAction == recvAction);
        }

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendAction.EventName = "Plot twist !";

            sendAction.GenerateDifferential(recvAction, writer);

            Buffer::Reader reader(&buff);
            recvAction.ApplyDifferential(reader);

            REQUIRE(sendAction == recvAction);
        }
    }

    GIVEN("A single cached event name")
    {
        ActionEvent sendAction, recvAction;

        TP_UNUSED(StringCache::Get().Add("test"))

        sendAction.ActionId = 42;
        sendAction.State1 = 6547;
        sendAction.Tick = 48;
        sendAction.ActorId = 12345678;
        sendAction.EventName = "test";
        sendAction.IdleId = 87964;
        sendAction.State2 = 8963;
        sendAction.TargetEventName = "toast";
        sendAction.TargetId = 963741;
        sendAction.Type = 4;

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendAction.GenerateDifferential(recvAction, writer);

            Buffer::Reader reader(&buff);
            recvAction.ApplyDifferential(reader);

            REQUIRE(sendAction == recvAction);
        }

        {
            Buffer buff(1000);
            Buffer::Writer writer(&buff);

            sendAction.EventName = "Plot twist !";

            sendAction.GenerateDifferential(recvAction, writer);

            Buffer::Reader reader(&buff);
            recvAction.ApplyDifferential(reader);

            REQUIRE(sendAction == recvAction);
        }
    }

    GIVEN("Full Mods")
    {
        Mods sendMods, recvMods;

        Buffer buff(1000);
        Buffer::Writer writer(&buff);

        sendMods.ModList.push_back({"Hello", 42});
        sendMods.ModList.push_back({"Hi", 14});
        sendMods.ModList.push_back({"Test", 8});
        sendMods.ModList.push_back({"Toast", 49});

        sendMods.Serialize(writer);

        Buffer::Reader reader(&buff);
        recvMods.Deserialize(reader);

        REQUIRE(sendMods == recvMods);
    }

    GIVEN("AnimationVariables")
    {
        AnimationVariables vars, recvVars;
 
        vars.Booleans.resize(76);
        String testString("\xDE\xAD\xBE\xEF"
                          "\xDE\xAD\xBE\xEF\x76\xB");
        vars.String_to_VectorBool(testString, vars.Booleans);

        vars.Floats.push_back(1.f);
        vars.Floats.push_back(7.f);
        vars.Floats.push_back(12.f);
        vars.Floats.push_back(0.f);
        vars.Floats.push_back(145.f);
        vars.Floats.push_back(100.f);
        vars.Floats.push_back(-1.f);

        vars.Integers.push_back(0);
        vars.Integers.push_back(12000);
        vars.Integers.push_back(06);
        vars.Integers.push_back(7778);
        vars.Integers.push_back(41104539);

        Buffer buff(1000);
        {
            Buffer::Writer writer(&buff);

            vars.GenerateDiff(recvVars, writer);

            Buffer::Reader reader(&buff);
            recvVars.ApplyDiff(reader);

            REQUIRE(vars.Booleans == recvVars.Booleans);
            REQUIRE(vars.Floats == recvVars.Floats);
            REQUIRE(vars.Integers == recvVars.Integers);
        }

        vars.Booleans.resize(33);
        vars.Booleans[16] = false;
        vars.Booleans[17] = false;
        vars.Booleans[18] = false;
        vars.Booleans[19] = false;
        vars.Floats[3] = 42.f;
        vars.Integers[0] = 18;
        vars.Integers[3] = 0;

        {
            Buffer::Writer writer(&buff);

            vars.GenerateDiff(recvVars, writer);

            Buffer::Reader reader(&buff);
            recvVars.ApplyDiff(reader);

            REQUIRE(vars.Booleans == recvVars.Booleans);
            REQUIRE(vars.Floats == recvVars.Floats);
            REQUIRE(vars.Integers == recvVars.Integers);
        }
    }
}

TEST_CASE("Packets", "[encoding.packets]")
{
    SECTION("AuthenticationRequest")
    {
        Buffer buff(1000);

        AuthenticationRequest sendMessage, recvMessage;
        sendMessage.Token = "TesSt";
        sendMessage.UserMods.ModList.push_back({"Hello", 42});
        sendMessage.UserMods.ModList.push_back({"Hi", 14});
        sendMessage.UserMods.ModList.push_back({"Test", 8});
        sendMessage.UserMods.ModList.push_back({"Toast", 49});

        Buffer::Writer writer(&buff);
        sendMessage.Serialize(writer);

        Buffer::Reader reader(&buff);

        uint64_t trash;
        reader.ReadBits(trash, 8); // pop opcode

        recvMessage.DeserializeRaw(reader);

        REQUIRE(sendMessage == recvMessage);
    }

    SECTION("AuthenticationResponse")
    {
        Buffer buff(1000);

        AuthenticationResponse sendMessage, recvMessage;
        sendMessage.Type = AuthenticationResponse::ResponseType::kAccepted;
        sendMessage.UserMods.ModList.push_back({"Hello", 42});
        sendMessage.UserMods.ModList.push_back({"Hi", 14});
        sendMessage.UserMods.ModList.push_back({"Test", 8});
        sendMessage.UserMods.ModList.push_back({"Toast", 49});

        Buffer::Writer writer(&buff);
        sendMessage.Serialize(writer);

        Buffer::Reader reader(&buff);

        uint64_t trash;
        reader.ReadBits(trash, 8); // pop opcode

        recvMessage.DeserializeRaw(reader);

        REQUIRE(sendMessage == recvMessage);
    }

    SECTION("AssignCharacterRequest")
    {
        Buffer buff(1000);

        ActionEvent sendAction;
        sendAction.ActionId = 42;
        sendAction.State1 = 6547;
        sendAction.Tick = 48;
        sendAction.ActorId = 12345678;
        sendAction.EventName = "test";
        sendAction.IdleId = 87964;
        sendAction.State2 = 8963;
        sendAction.TargetEventName = "toast";
        sendAction.TargetId = 963741;
        sendAction.Type = 4;

        AssignCharacterRequest sendMessage, recvMessage;
        sendMessage.Cookie = 14523698;
        sendMessage.AppearanceBuffer = "toto";
        sendMessage.CellId.BaseId = 45;
        sendMessage.FormId.ModId = 48;
        sendMessage.ReferenceId.BaseId = 456799;
        sendMessage.ReferenceId.ModId = 4079;
        sendMessage.LatestAction = sendAction;
        sendMessage.Position.x = -452.4f;
        sendMessage.Position.y = 452.4f;
        sendMessage.Position.z = 125452.4f;
        sendMessage.Rotation.x = -1.87f;
        sendMessage.Rotation.y = 45.35f;

        Buffer::Writer writer(&buff);
        sendMessage.Serialize(writer);

        Buffer::Reader reader(&buff);

        uint64_t trash;
        reader.ReadBits(trash, 8); // pop opcode

        recvMessage.DeserializeRaw(reader);

        REQUIRE(sendMessage == recvMessage);
    }

    GIVEN("ClientReferencesMoveRequest")
    {
        ClientReferencesMoveRequest sendMessage, recvMessage;
        sendMessage.Tick = 1712345678901;
        auto& move = sendMessage.Updates[1];
        move.Position.x = 1234.5f;
        move.Direction = 0.75f;

        AnimationVariables vars;
        vars.Booleans.resize(76);
        String testString("\xDE\xAD\xBE\xEF\x76\xB");
        vars.String_to_VectorBool(testString, vars.Booleans);

        vars.Floats.push_back(1.f);
        vars.Floats.push_back(7.f);
        vars.Floats.push_back(12.f);
        vars.Floats.push_back(0.f);
        vars.Floats.push_back(145.f);
        vars.Floats.push_back(100.f);
        vars.Floats.push_back(-1.f);

        vars.Integers.push_back(0);
        vars.Integers.push_back(12000);
        vars.Integers.push_back(06);
        vars.Integers.push_back(7778);
        vars.Integers.push_back(41104539);

        move.Variables = vars;

        Buffer buff(1000);
        Buffer::Writer writer(&buff);
        sendMessage.Serialize(writer);

        Buffer::Reader reader(&buff);

        uint64_t trash;
        reader.ReadBits(trash, 8); // pop opcode

        recvMessage.DeserializeRaw(reader);

        REQUIRE(recvMessage.Tick == sendMessage.Tick);
        REQUIRE(recvMessage.Updates[1] == sendMessage.Updates[1]);
    }

    GIVEN("RequestActionEvents")
    {
        ActionEvent firstAction;
        firstAction.Tick = 1712345678901;
        firstAction.ActionId = 42;
        firstAction.TargetId = 963741;
        firstAction.State1 = 6547;
        firstAction.EventName = "jumpStandingStart";

        ActionEvent secondAction = firstAction;
        secondAction.Tick += 120;
        secondAction.ActionId = 43;
        secondAction.EventName = "attackStart";
        secondAction.TargetEventName = "toast";

        RequestActionEvents sendMessage, recvMessage;
        sendMessage.Actions[1] = {firstAction, secondAction};
        sendMessage.Actions[7] = {secondAction};

        Buffer buff(1000);
        Buffer::Writer writer(&buff);
        sendMessage.Serialize(writer);

        Buffer::Reader reader(&buff);

        uint64_t trash;
        reader.ReadBits(trash, 8); // pop opcode

        recvMessage.DeserializeRaw(reader);

        REQUIRE(sendMessage == recvMessage);
    }

    GIVEN("NotifyActionEvents")
    {
        ActionEvent action;
        action.Tick = 1712345678901;
        action.ActionId = 42;
        action.EventName = "blockStart";

        NotifyActionEvents sendMessage;
        sendMessage.Actions[3] = {action, action};

        Buffer buff(1000);
        Buffer::Writer writer(&buff);
        sendMessage.Serialize(writer);

        Buffer::Reader reader(&buff);

        const ServerMessageFactory factory;
        auto pMessage = factory.Extract(reader);

        REQUIRE(pMessage);
        REQUIRE(pMessage->GetOpcode() == sendMessage.GetOpcode());
        REQUIRE(*CastUnique<NotifyActionEvents>(std::move(pMessage)) == sendMessage);
    }

    GIVEN("Dropped item messages")
    {
        DroppedItemData data;
        data.ServerId = 1234;
        data.Item.BaseId = GameId(1, 0x12EB7);
        data.Item.Count = 3;
        data.Item.ExtraHealth = 1.5f;
        data.CellId = GameId(0, 0x9732);
        data.WorldSpaceId = GameId(0, 0x3C);
        data.PositionX = -1024.5f;
        data.PositionY = 2048.25f;
        data.PositionZ = 12.f;
        data.RotationZ = 1.57f;

        Buffer buff(1000);

        {
            DropItemRequest sendMessage;
            sendMessage.RefId = 0xFF001234;
            sendMessage.Data = data;

            Buffer::Writer writer(&buff);
            sendMessage.Serialize(writer);
            Buffer::Reader reader(&buff);

            const ClientMessageFactory factory;
            auto pMessage = factory.Extract(reader);
            REQUIRE(pMessage);
            REQUIRE(*CastUnique<DropItemRequest>(std::move(pMessage)) == sendMessage);
        }

        {
            PickupDroppedItemRequest sendMessage;
            sendMessage.ServerId = 77;

            Buffer::Writer writer(&buff);
            sendMessage.Serialize(writer);
            Buffer::Reader reader(&buff);

            const ClientMessageFactory factory;
            auto pMessage = factory.Extract(reader);
            REQUIRE(pMessage);
            REQUIRE(*CastUnique<PickupDroppedItemRequest>(std::move(pMessage)) == sendMessage);
        }

        {
            NotifyDroppedItems sendMessage;
            sendMessage.Items.push_back(data);
            data.ServerId = 99;
            sendMessage.Items.push_back(data);

            Buffer::Writer writer(&buff);
            sendMessage.Serialize(writer);
            Buffer::Reader reader(&buff);

            const ServerMessageFactory factory;
            auto pMessage = factory.Extract(reader);
            REQUIRE(pMessage);
            REQUIRE(*CastUnique<NotifyDroppedItems>(std::move(pMessage)) == sendMessage);
        }

        {
            DropItemResponse sendMessage;
            sendMessage.RefId = 0xFF001234;
            sendMessage.ServerId = 1234;

            Buffer::Writer writer(&buff);
            sendMessage.Serialize(writer);
            Buffer::Reader reader(&buff);

            const ServerMessageFactory factory;
            auto pMessage = factory.Extract(reader);
            REQUIRE(pMessage);
            REQUIRE(*CastUnique<DropItemResponse>(std::move(pMessage)) == sendMessage);
        }

        {
            NotifyDroppedItemRemoved sendMessage;
            sendMessage.ServerId = 1234;

            Buffer::Writer writer(&buff);
            sendMessage.Serialize(writer);
            Buffer::Reader reader(&buff);

            const ServerMessageFactory factory;
            auto pMessage = factory.Extract(reader);
            REQUIRE(pMessage);
            REQUIRE(*CastUnique<NotifyDroppedItemRemoved>(std::move(pMessage)) == sendMessage);
        }

        {
            PickupDroppedItemResponse sendMessage;
            sendMessage.ServerId = 1234;
            sendMessage.Accepted = true;

            Buffer::Writer writer(&buff);
            sendMessage.Serialize(writer);
            Buffer::Reader reader(&buff);

            const ServerMessageFactory factory;
            auto pMessage = factory.Extract(reader);
            REQUIRE(pMessage);
            REQUIRE(*CastUnique<PickupDroppedItemResponse>(std::move(pMessage)) == sendMessage);
        }
    }

    GIVEN("AssignObjectsResponse with container flags")
    {
        ObjectData object;
        object.ServerId = 5;
        object.Id = GameId(0, 0x1234);
        object.IsContainer = true;
        object.MergeInventory = true;
        Inventory::Entry entry;
        entry.BaseId = GameId(0, 0xF);
        entry.Count = 100;
        object.CurrentInventory.Entries.push_back(entry);

        AssignObjectsResponse sendMessage;
        sendMessage.Objects.push_back(object);

        Buffer buff(1000);
        Buffer::Writer writer(&buff);
        sendMessage.Serialize(writer);
        Buffer::Reader reader(&buff);

        const ServerMessageFactory factory;
        auto pMessage = factory.Extract(reader);
        REQUIRE(pMessage);
        REQUIRE(*CastUnique<AssignObjectsResponse>(std::move(pMessage)) == sendMessage);
    }
}

TEST_CASE("Container inventories", "[encoding.inventory]")
{
    Inventory::Entry gold;
    gold.BaseId = GameId(0, 0xF);
    gold.Count = 100;

    Inventory::Entry sword;
    sword.BaseId = GameId(0, 0x12EB7);
    sword.Count = 1;

    SECTION("Clamped removal never records negative entries")
    {
        Inventory inventory;

        Inventory::Entry removal = sword;
        removal.Count = -1;
        inventory.AddOrRemoveEntryClamped(removal);
        REQUIRE(inventory.Entries.empty());

        inventory.AddOrRemoveEntryClamped(gold);
        Inventory::Entry goldRemoval = gold;
        goldRemoval.Count = -150;
        inventory.AddOrRemoveEntryClamped(goldRemoval);
        REQUIRE(inventory.Entries.empty());
    }

    SECTION("Merging adds only what the other copy has more of")
    {
        // Server state: the host's gold plus a sword another player stored.
        Inventory server;
        server.Entries.push_back(gold);
        server.Entries.push_back(sword);

        // Entering player's local copy: less gold, the sword is missing, but a helmet they stored offline.
        Inventory::Entry helmet;
        helmet.BaseId = GameId(0, 0x12E4D);
        helmet.Count = 2;

        Inventory local;
        Inventory::Entry localGold = gold;
        localGold.Count = 30;
        local.Entries.push_back(localGold);
        local.Entries.push_back(helmet);

        auto added = server.MergeMissing(local);

        REQUIRE(added.size() == 1);
        REQUIRE(added[0].BaseId == helmet.BaseId);
        REQUIRE(added[0].Count == 2);

        REQUIRE(server.GetEntryCountById(gold.BaseId) == 100);
        REQUIRE(server.GetEntryCountById(sword.BaseId) == 1);
        REQUIRE(server.GetEntryCountById(helmet.BaseId) == 2);

        // More of an existing item only adds the difference.
        Inventory richer;
        Inventory::Entry moreGold = gold;
        moreGold.Count = 250;
        richer.Entries.push_back(moreGold);

        added = server.MergeMissing(richer);
        REQUIRE(added.size() == 1);
        REQUIRE(added[0].Count == 150);
        REQUIRE(server.GetEntryCountById(gold.BaseId) == 250);

        // Split stacks of the same item count as one.
        Inventory split;
        Inventory::Entry stack = gold;
        stack.Count = 200;
        split.Entries.push_back(stack);
        split.Entries.push_back(stack);

        added = server.MergeMissing(split);
        REQUIRE(added.size() == 1);
        REQUIRE(added[0].Count == 150);
        REQUIRE(server.GetEntryCountById(gold.BaseId) == 400);
    }
}

TEST_CASE("StringCache", "[encoding.string_cache]")
{
    SECTION("Messages")
    {
        StringCacheUpdate update;
        update.Values.push_back("Hello");
        update.Values.push_back("Bye");

        Buffer buff(1000);
        Buffer::Writer writer(&buff);
        update.Serialize(writer);

        Buffer::Reader reader(&buff);

        uint64_t trash;
        reader.ReadBits(trash, 8); // pop opcode

        StringCacheUpdate recvUpdate;
        recvUpdate.DeserializeRaw(reader);

        REQUIRE(update == recvUpdate);
    }
}
