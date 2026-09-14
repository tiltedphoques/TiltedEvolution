#include <Services/ObjectLifecycle.h>

#include <Components.h>
#include <Messages/NotifyRemoveObjects.h>

#include <algorithm>

namespace ObjectLifecycle
{
void RetireCell(entt::registry& aRegistry, const GameId& acCell, std::span<const GameId> aPlayerCells, const SendRemoval& aSendRemoval) noexcept
{
    // Preserve the existing policy: occupied current cells are retained.
    // Tracking all loaded exterior cells remains a separate concern.
    if (std::find(aPlayerCells.begin(), aPlayerCells.end(), acCell) != aPlayerCells.end())
        return;

    const auto objectView = aRegistry.view<ObjectComponent, CellIdComponent>();
    Vector<entt::entity> toDestroy;
    NotifyRemoveObjects notify{};

    for (const auto entity : objectView)
    {
        if (objectView.get<CellIdComponent>(entity).Cell != acCell)
            continue;

        toDestroy.push_back(entity);
        notify.ServerIds.push_back(entt::to_integral(entity));
    }

    if (!notify.ServerIds.empty())
        aSendRemoval(notify);

    for (const auto entity : toDestroy)
        aRegistry.destroy(entity);
}
} // namespace ObjectLifecycle
