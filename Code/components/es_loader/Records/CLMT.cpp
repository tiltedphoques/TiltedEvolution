#include "CLMT.h"

#include <ESLoader.h>

CLMT CLMT::ParseChunks() noexcept
{
    CLMT clmt;
    clmt.CopyRecordData(*this);

    IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) { 
        switch (aChunkId)
        {
        case ChunkId::EDID_ID:
            clmt.m_editorId = ESLoader::ReadZString(aReader);
            break;
        case ChunkId::WLST_ID:
            clmt.m_weatherList = Chunks::WLST(aReader);
            break;
        case ChunkId::FNAM_ID:
            clmt.m_sunTexture = ESLoader::ReadZString(aReader);
            break;
        case ChunkId::GNAM_ID:
            clmt.m_glareTexture = ESLoader::ReadZString(aReader);
            break;
        case ChunkId::TNAM_ID:
            clmt.m_timing = Chunks::TNAM(aReader);
            break;
        }
    });

    return clmt;
}
