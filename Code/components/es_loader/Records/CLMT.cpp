#include "CLMT.h"

#include <ESLoader.h>

void CLMT::ParseChunks(CLMT& aSourceRecord, Map<uint8_t, uint32_t>& aParentToFormIdPrefix) noexcept
{
    aSourceRecord.IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) { 
        switch (aChunkId)
        {
        case ChunkId::EDID_ID:
            m_editorId = ESLoader::ReadZString(aReader);
            break;
        case ChunkId::WLST_ID:
            m_weatherList = Chunks::WLST(aReader, aParentToFormIdPrefix);
            break;
        case ChunkId::FNAM_ID:
            m_sunTexture = ESLoader::ReadZString(aReader);
            break;
        case ChunkId::GNAM_ID:
            m_glareTexture = ESLoader::ReadZString(aReader);
            break;
        case ChunkId::TNAM_ID:
            m_timing = Chunks::TNAM(aReader);
            break;
        }
    });
}
