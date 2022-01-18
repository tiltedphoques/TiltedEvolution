#include "NPC.h"

#include <ESLoader.h>

NPC::Data NPC::ParseChunks() noexcept
{
    Data data;

    IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) {
        switch (aChunkId)
        {
        case ChunkId::ACBS_ID:
            data.m_baseStats = Chunks::ACBS(aReader);
            break;
        case ChunkId::DOFT_ID:
            data.m_defaultOutfit = Chunks::DOFT(aReader);
            break;
        }
    });

    return data;
}
