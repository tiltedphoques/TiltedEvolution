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
            // XMRK contains no data
            m_markerData.m_isMarker = true;
            break;
        case ChunkId::FNAM_ID:
            aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_markerData.m_flags), 1);
            break;
        case ChunkId::TNAM_ID:
            aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_markerData.m_marker), 2);
            break;
        }
    });
}
