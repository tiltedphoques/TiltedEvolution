#include "REFR.h"

REFR REFR::ParseChunks() noexcept
{
    REFR refr;
    refr.CopyRecordData(*this);

    IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) { 
        switch (aChunkId)
        {
        case ChunkId::NAME_ID:
            refr.m_basicObject = Chunks::NAME(aReader);
            break;
        }
    });

    return refr;
}
