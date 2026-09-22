#include <catch2/catch.hpp>

#include <Components.h>
#include <Messages/NotifyRemoveObjects.h>
#include <Services/ObjectLifecycle.h>

#include <algorithm>
#include <array>

namespace
{
entt::entity CreateObject(entt::registry& aRegistry, const GameId& acCell)
{
    const auto entity = aRegistry.create();
    aRegistry.emplace<ObjectComponent>(entity, nullptr);
    aRegistry.emplace<CellIdComponent>(entity, acCell);
    return entity;
}

struct RetirementRecorder
{
    explicit RetirementRecorder(entt::registry& aRegistry)
        : Registry(aRegistry)
    {
    }

    void operator()(const NotifyRemoveObjects& acMessage)
    {
        for (const auto id : acMessage.ServerIds)
        {
            const auto entity = static_cast<entt::entity>(id);
            ObjectsAliveAtSend &= Registry.valid(entity) && Registry.all_of<ObjectComponent, CellIdComponent>(entity);
        }

        Messages.push_back(acMessage);
    }

    entt::registry& Registry;
    Vector<NotifyRemoveObjects> Messages;
    bool ObjectsAliveAtSend{true};
};
} // namespace

TEST_CASE("An occupied cell retains server objects and their lock state", "[server.objects]")
{
    entt::registry registry;
    const GameId cell{1, 0x100};
    const auto object = CreateObject(registry, cell);
    auto& lock = registry.get<ObjectComponent>(object).CurrentLockData;
    lock.IsLocked = true;
    lock.LockLevel = 75;

    RetirementRecorder sent(registry);
    const std::array<GameId, 3> playerCells{GameId{1, 0x200}, GameId{1, 0x300}, cell};
    ObjectLifecycle::RetireCell(registry, cell, playerCells, std::ref(sent));

    REQUIRE(sent.Messages.empty());
    REQUIRE(registry.valid(object));
    REQUIRE(registry.get<ObjectComponent>(object).CurrentLockData.IsLocked);
    REQUIRE(registry.get<ObjectComponent>(object).CurrentLockData.LockLevel == 75);
}

TEST_CASE("The last departure retires only objects belonging to the complete cell id", "[server.objects]")
{
    entt::registry registry;
    const GameId cell{1, 0x100};
    const auto first = CreateObject(registry, cell);
    const auto second = CreateObject(registry, cell);
    const auto otherCell = CreateObject(registry, GameId{1, 0x200});
    const auto otherMod = CreateObject(registry, GameId{2, 0x100});
    const auto actor = registry.create();
    registry.emplace<CellIdComponent>(actor, cell);
    const auto noCell = registry.create();
    registry.emplace<ObjectComponent>(noCell, nullptr);

    // Former visitors are now outside this cell, including one with the same
    // base cell id in a different mod. They must not prevent its retirement.
    const std::array<GameId, 2> playerCells{GameId{1, 0x200}, GameId{2, 0x100}};
    RetirementRecorder sent(registry);
    ObjectLifecycle::RetireCell(registry, cell, playerCells, std::ref(sent));

    REQUIRE(sent.Messages.size() == 1);
    auto actual = sent.Messages.front().ServerIds;
    std::sort(actual.begin(), actual.end());
    Vector<uint32_t> expected{entt::to_integral(first), entt::to_integral(second)};
    std::sort(expected.begin(), expected.end());
    REQUIRE(actual == expected);
    REQUIRE(sent.ObjectsAliveAtSend);
    REQUIRE_FALSE(registry.valid(first));
    REQUIRE_FALSE(registry.valid(second));
    REQUIRE(registry.valid(otherCell));
    REQUIRE(registry.valid(otherMod));
    REQUIRE(registry.valid(actor));
    REQUIRE(registry.valid(noCell));
}

TEST_CASE("An empty cell does not broadcast an empty retirement", "[server.objects]")
{
    entt::registry registry;
    const GameId cell{1, 0x100};
    const auto other = CreateObject(registry, GameId{1, 0x200});
    RetirementRecorder sent(registry);

    ObjectLifecycle::RetireCell(registry, cell, {}, std::ref(sent));

    REQUIRE(sent.Messages.empty());
    REQUIRE(registry.valid(other));
}

TEST_CASE("Repeated departure events are harmless after all players disconnect", "[server.objects]")
{
    entt::registry registry;
    const GameId cell{1, 0x100};
    const auto object = CreateObject(registry, cell);
    RetirementRecorder sent(registry);

    ObjectLifecycle::RetireCell(registry, cell, {}, std::ref(sent));
    REQUIRE(sent.Messages.size() == 1);
    REQUIRE(sent.Messages.front().ServerIds == Vector<uint32_t>{entt::to_integral(object)});
    REQUIRE(sent.ObjectsAliveAtSend);
    REQUIRE_FALSE(registry.valid(object));

    ObjectLifecycle::RetireCell(registry, cell, {}, std::ref(sent));
    REQUIRE(sent.Messages.size() == 1);
}

TEST_CASE("Server retirement preserves entity generation bits after reentry", "[server.objects]")
{
    entt::registry registry;
    const GameId cell{1, 0x100};
    const auto original = CreateObject(registry, cell);
    RetirementRecorder sent(registry);

    ObjectLifecycle::RetireCell(registry, cell, {}, std::ref(sent));
    const auto replacement = CreateObject(registry, cell);
    REQUIRE(entt::to_entity(original) == entt::to_entity(replacement));
    REQUIRE(entt::to_integral(original) != entt::to_integral(replacement));

    const std::array<GameId, 1> playerCells{cell};
    ObjectLifecycle::RetireCell(registry, cell, playerCells, std::ref(sent));
    REQUIRE(registry.valid(replacement));
    REQUIRE(sent.Messages.size() == 1);

    ObjectLifecycle::RetireCell(registry, cell, {}, std::ref(sent));
    REQUIRE(sent.Messages.size() == 2);
    REQUIRE(sent.Messages.back().ServerIds == Vector<uint32_t>{entt::to_integral(replacement)});
    REQUIRE(sent.ObjectsAliveAtSend);
    REQUIRE_FALSE(registry.valid(replacement));
}
