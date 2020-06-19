#include "catch.hpp"
#include <Messages/ClientMessageFactory.h>
#include <Messages/AuthenticationRequest.h>
#include <Messages/AuthenticationResponse.h>
#include <Structs/ActionEvent.h>
#include <Structs/Mods.h>

using namespace TiltedPhoques;

TEST_CASE("Encoding factory", "[encoding.factory]")
{
    Buffer buff(1000);

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

    GIVEN("Full Mods")
    {
        Mods sendMods, recvMods;

        Buffer buff(1000);
        Buffer::Writer writer(&buff);

        sendMods.StandardMods.push_back({ "Hello", 42 });
        sendMods.StandardMods.push_back({ "Hi", 14 });
        sendMods.LiteMods.push_back({ "Test", 8 });
        sendMods.LiteMods.push_back({ "Toast", 49 });

        sendMods.Serialize(writer);

        Buffer::Reader reader(&buff);
        recvMods.Deserialize(reader);

        REQUIRE(sendMods == recvMods);
    }
}

TEST_CASE("Packets", "[encoding.packets]")
{
    SECTION("AuthenticationRequest")
    {
        Buffer buff(1000);

        AuthenticationRequest sendMessage, recvMessage;
        sendMessage.Token = "TesSt";
        sendMessage.Mods.StandardMods.push_back({ "Hello", 42 });
        sendMessage.Mods.StandardMods.push_back({ "Hi", 14 });
        sendMessage.Mods.LiteMods.push_back({ "Test", 8 });
        sendMessage.Mods.LiteMods.push_back({ "Toast", 49 });

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
        sendMessage.Accepted = true;
        sendMessage.Mods.StandardMods.push_back({ "Hello", 42 });
        sendMessage.Mods.StandardMods.push_back({ "Hi", 14 });
        sendMessage.Mods.LiteMods.push_back({ "Test", 8 });
        sendMessage.Mods.LiteMods.push_back({ "Toast", 49 });
        sendMessage.Scripts.push_back(1);
        sendMessage.Scripts.push_back(2);
        sendMessage.ReplicatedObjects.push_back(3);
        sendMessage.ReplicatedObjects.push_back(4);

        Buffer::Writer writer(&buff);
        sendMessage.Serialize(writer);

        Buffer::Reader reader(&buff);

        uint64_t trash;
        reader.ReadBits(trash, 8); // pop opcode

        recvMessage.DeserializeRaw(reader);

        REQUIRE(sendMessage == recvMessage);
    }
}
