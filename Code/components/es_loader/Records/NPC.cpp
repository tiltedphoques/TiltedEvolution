#include "NPC.h"

#include <ESLoader.h>

void NPC::ParseChunks() noexcept
{
    IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) {
        switch (aChunkId)
        {
        case ChunkId::ACBS_ID:
            m_baseStats = Chunks::ACBS(aReader);
            break;
        case ChunkId::DOFT_ID:
            m_defaultOutfit = Chunks::DOFT(aReader);
            break;
        }
    });
}
