#include <TiltedCore/Stl.hpp>
#include <TiltedCore/Buffer.hpp>
#include <TiltedCore/Serialization.hpp>
#include <TiltedCore/Outcome.hpp>
#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <catch2/catch.hpp>
#include <array>
#include <optional>

using TiltedPhoques::List;
using TiltedPhoques::Outcome;
using TiltedPhoques::Vector;

#include <Components.h>
#include <Systems/ObjectSystem.h>

TEST_CASE("Object bindings follow reassignment without duplicating forms", "[objects]")
{
    entt::registry registry;
    const auto object = ObjectSystem::Setup(registry, 0x1234, 17);

    SECTION("Repeated assignment keeps the same entity")
    {
        REQUIRE(ObjectSystem::Setup(registry, 0x1234, 17) == object);
        REQUIRE(registry.view<ObjectComponent>().size() == 1);
    }

    SECTION("Returning to a retired cell changes the server id")
    {
        REQUIRE(ObjectSystem::Setup(registry, 0x1234, 0x100011) == object);
        REQUIRE(registry.get<ObjectComponent>(object).Id == 0x100011);
        REQUIRE(registry.view<ObjectComponent>().size() == 1);

        // A late or duplicate removal of the old incarnation must not erase the new one.
        ObjectSystem::Remove(registry, std::array<uint32_t, 1>{17});
        REQUIRE(registry.valid(object));
        REQUIRE(registry.get<ObjectComponent>(object).Id == 0x100011);
    }

    SECTION("A server id reused for another form retires the previous binding")
    {
        const auto replacement = ObjectSystem::Setup(registry, 0x5678, 17);
        REQUIRE_FALSE(registry.valid(object));
        REQUIRE(registry.get<FormIdComponent>(replacement).Id == 0x5678);
        REQUIRE(registry.view<ObjectComponent>().size() == 1);
    }

    SECTION("Legacy duplicates are collapsed on reassignment")
    {
        const auto duplicate = registry.create();
        registry.emplace<FormIdComponent>(duplicate, 0x1234u);
        registry.emplace<ObjectComponent>(duplicate, 99);

        const auto assigned = ObjectSystem::Setup(registry, 0x1234, 42);
        REQUIRE(registry.valid(assigned));
        REQUIRE(registry.get<ObjectComponent>(assigned).Id == 42);
        REQUIRE(registry.view<ObjectComponent>().size() == 1);
    }
}

TEST_CASE("Object removal preserves unrelated entities and tolerates repeated messages", "[objects]")
{
    entt::registry registry;
    const auto first = ObjectSystem::Setup(registry, 0x1234, 17);
    const auto second = ObjectSystem::Setup(registry, 0x5678, 18);
    const auto actor = registry.create();
    registry.emplace<FormIdComponent>(actor, 0x14u);
    registry.emplace<PlayerComponent>(actor, 17);

    ObjectSystem::Remove(registry, std::array<uint32_t, 3>{17, 17, 999});
    REQUIRE_FALSE(registry.valid(first));
    REQUIRE(registry.valid(second));
    REQUIRE(registry.valid(actor));
    REQUIRE(registry.view<ObjectComponent>().size() == 1);

    ObjectSystem::Remove(registry, std::array<uint32_t, 1>{17});
    ObjectSystem::Remove(registry, {});
    REQUIRE(registry.valid(second));
    REQUIRE(registry.valid(actor));
}

TEST_CASE("Disconnect clears both sides of object bindings before reconnect", "[objects]")
{
    entt::registry registry;
    const auto first = ObjectSystem::Setup(registry, 0x1234, 17);
    const auto second = ObjectSystem::Setup(registry, 0x5678, 18);
    const auto actor = registry.create();
    registry.emplace<FormIdComponent>(actor, 0x14u);

    ObjectSystem::Clear(registry);
    REQUIRE_FALSE(registry.valid(first));
    REQUIRE_FALSE(registry.valid(second));
    REQUIRE(registry.valid(actor));
    REQUIRE(registry.view<ObjectComponent>().empty());
    REQUIRE(registry.view<FormIdComponent>().size() == 1);

    ObjectSystem::Clear(registry);
    const auto reconnected = ObjectSystem::Setup(registry, 0x5678, 17);
    REQUIRE(registry.get<FormIdComponent>(reconnected).Id == 0x5678);
    REQUIRE(registry.get<ObjectComponent>(reconnected).Id == 17);
    REQUIRE(registry.view<ObjectComponent>().size() == 1);
}
