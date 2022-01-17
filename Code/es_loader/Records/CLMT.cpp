#include "CLMT.h"

#include <ESLoader.h>

CLMT::Data CLMT::ParseChunks() noexcept
{
    Data data;

    IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) { 
        switch (aChunkId)
        {
        case ChunkId::EDID_ID:
            data.m_editorId = ESLoader::LoadZString(aReader);
            break;
        case ChunkId::WLST_ID:
            data.m_weatherList = Chunks::WLST(aReader);
            break;
        case ChunkId::FNAM_ID:
            data.m_sunTexture = ESLoader::LoadZString(aReader);
            break;
        case ChunkId::GNAM_ID:
            data.m_glareTexture = ESLoader::LoadZString(aReader);
            break;
        case ChunkId::TNAM_ID:
            data.m_timing = Chunks::TNAM(aReader);
            break;
        }
    });

    return data;
}
