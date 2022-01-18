#include "REFR.h"

REFR::Data REFR::ParseChunks() noexcept
{
    Data data;

    IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) { 
        switch (aChunkId)
        {
        case ChunkId::NAME_ID:
            data.m_basicObject = Chunks::NAME(aReader);
            break;
        }
    });

    return data;
}
