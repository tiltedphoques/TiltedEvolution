#include "REFR.h"

void REFR::ParseChunks(REFR& aSourceRecord) noexcept
{
    aSourceRecord.IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) { 
        switch (aChunkId)
        {
        case ChunkId::NAME_ID:
            m_basicObject = Chunks::NAME(aReader);
            break;
        case ChunkId::XMRK_ID:
            break;
        case ChunkId::FNAM_ID:
            break;
        }
    });
}
