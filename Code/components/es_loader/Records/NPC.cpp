#include "NPC.h"

#include <ESLoader.h>

NPC NPC::ParseChunks() noexcept
{
    NPC npc;
    npc.CopyRecordData(*this);

    IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) {
        switch (aChunkId)
        {
        case ChunkId::ACBS_ID:
            npc.m_baseStats = Chunks::ACBS(aReader);
            break;
        case ChunkId::DOFT_ID:
            npc.m_defaultOutfit = Chunks::DOFT(aReader);
            break;
        }
    });

    return npc;
}
