#include <Services/DatabaseService.h>

#include <GameServer.h>
#include <World.h>
#include <Components.h>

#include <Events/ObjectAddedEvent.h>
#include <Events/ObjectUpdatedEvent.h>

Console::StringSetting sPersistenceHost{"Persistence:sHost", "Host name/IP of the database", "127.0.0.1"};
Console::Setting uPersistencePort{"Persistence:uPort", "Host name/IP of the database", 3306u};
Console::StringSetting sPersistenceUsername{"Persistence:sUsername", "Database username", "root"};
Console::StringSetting sPersistencePassword{"Persistence:sPassword", "Database password", "password"};
Console::StringSetting sPersistenceDatabase{"Persistence:sDatabase", "Database", "skyrim_together"};

DatabaseService::DatabaseService(World& aWorld, entt::dispatcher& aDispatcher) 
    : m_world(aWorld)
    , m_db(sPersistenceHost.c_str(), uPersistencePort.value_as<uint16_t>(), sPersistenceUsername.c_str(),
           sPersistencePassword.c_str(), sPersistenceDatabase.c_str(), 10)
{
    m_objectUpdatedConnection = aDispatcher.sink<ObjectUpdatedEvent>().connect<&DatabaseService::OnObjectUpdatedEvent>(this);
    m_objectAddedConnection = aDispatcher.sink<ObjectAddedEvent>().connect<&DatabaseService::OnObjectAddedEvent>(this);

    LoadObjects();
}

void DatabaseService::LoadObjects()
{
    auto result = m_db.Fetch("SELECT * FROM objects;").get();
    if (!result)
    {
        spdlog::error("Unable to load objects! Check that the database settings are populated correctly.");
        return;
    }

    const auto& lines = *result;
    for (const auto& row : lines)
    {
        const auto cEntity = m_world.create();

        m_world.emplace<FormIdComponent>(cEntity, GameId(*row[0].As<uint64_t>()));

        auto& objectComponent = m_world.emplace<ObjectComponent>(cEntity, nullptr);
        objectComponent.CurrentLockData.IsLocked = *row[1].As<bool>();
        objectComponent.CurrentLockData.LockLevel = *row[2].As<uint64_t>() & 0xFF;

        m_world.emplace<CellIdComponent>(cEntity, GameId(*row[3].As<uint64_t>()), GameId(*row[4].As<uint64_t>()),
                                         GridCellCoords(GameId(*row[5].As<int32_t>()), GameId(*row[6].As<int32_t>())));

        auto& inventoryComp = m_world.emplace<InventoryComponent>(cEntity);
    }

    spdlog::info("Loaded {} objects from database.", lines.size());
}

void DatabaseService::OnObjectUpdatedEvent(const ObjectUpdatedEvent& acEvent) noexcept
{
    const auto view = m_world.view<FormIdComponent, ObjectComponent>();
    auto itor = view.find(acEvent.Entity);
    if (itor == std::end(view))
    {
        spdlog::error("Failed to retrieve object {:X} when writing to database", World::ToInteger(acEvent.Entity));
        return;
    }

    auto [formIdComponent, objectComponent] = view.get(*itor);

    auto query = fmt::format("UPDATE objects SET is_locked = {}, lock_level = {} WHERE game_id = {}", objectComponent.CurrentLockData.IsLocked ? 1 : 0,
                             (uint32_t)objectComponent.CurrentLockData.LockLevel, formIdComponent.Id.AsUint64());

    // Discard future
    m_db.Execute(query);
}

void DatabaseService::OnObjectAddedEvent(const ObjectAddedEvent& acEvent) noexcept
{
    const auto view = m_world.view<FormIdComponent, ObjectComponent, CellIdComponent>();
    auto itor = view.find(acEvent.Entity);
    if (itor == std::end(view))
    {
        spdlog::error("Failed to retrieve object {:X} when writing to database", World::ToInteger(acEvent.Entity));
        return;
    }

    auto [formIdComponent, objectComponent, cellIdComponent] = view.get(*itor);

    auto query = fmt::format("INSERT INTO objects VALUES({}, {}, {}, {}, {}, {}, {})", 
        formIdComponent.Id.AsUint64(),
        objectComponent.CurrentLockData.IsLocked ? 1 : 0,
        (uint32_t)objectComponent.CurrentLockData.LockLevel,
        cellIdComponent.Cell.AsUint64(),
        cellIdComponent.WorldSpaceId.AsUint64(),
        cellIdComponent.CenterCoords.X,
        cellIdComponent.CenterCoords.Y);

    // Discard future
    m_db.Execute(query);
}
