#include "RecordCollection.h"

namespace ESLoader
{
void RecordCollection::BuildReferences()
{
    for (auto& [_, navmesh] : m_navMeshes)
    {
        if (navmesh.m_navMesh.m_worldSpaceId)
        {
            auto& world = GetWorldById(navmesh.m_navMesh.m_worldSpaceId);
            world.m_navMeshRefs.push_back(&navmesh);
        }
    }
}
} // namespace ESLoader
