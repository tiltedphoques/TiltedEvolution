#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Allocator.hpp>
#include <TiltedCore/Buffer.hpp>
#include <TiltedCore/Serialization.hpp>
#include <optional>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <catch2/catch.hpp>

#include <Messages/NotifyRemoveObjects.h>
#include <Messages/ServerMessageFactory.h>

using namespace TiltedPhoques;

TEST_CASE("Object removal notifications preserve server entity identifiers", "[encoding.objects]")
{
    NotifyRemoveObjects sent;

    SECTION("Empty notification")
    {
    }

    SECTION("Entity indices and generation bits")
    {
        sent.ServerIds = {0, 1, 0x100000, 0xABC12345, 0xFFFFFFFE};
    }

    SECTION("A cell containing many objects")
    {
        for (uint32_t i = 0; i < 1024; ++i)
            sent.ServerIds.push_back(0xABC00000 + i);
    }

    Buffer buffer(16384);
    Buffer::Writer writer(&buffer);
    sent.Serialize(writer);

    Buffer::Reader reader(&buffer);
    const ServerMessageFactory factory;
    auto message = factory.Extract(reader);

    REQUIRE(message);
    REQUIRE(message->GetOpcode() == NotifyRemoveObjects::Opcode);
    auto received = CastUnique<NotifyRemoveObjects>(std::move(message));
    REQUIRE(*received == sent);
}
