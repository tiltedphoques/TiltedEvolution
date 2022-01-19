#include "REFR.h"

void REFR::ParseChunks() noexcept
{
    IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) { 
        switch (aChunkId)
        {
        case ChunkId::NAME_ID:
            m_basicObject = Chunks::NAME(aReader);
            break;
        }
    });
}
