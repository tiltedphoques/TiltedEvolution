#include "NAVM.h"

#include <ESLoader.h>

void NAVM::ParseChunks(NAVM &aSourceRecord, Map<uint8_t, uint32_t>& aParentToFormIdPrefix) noexcept
{
    aSourceRecord.IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) { 
        switch (aChunkId)
        {
        case ChunkId::NVNM_ID:
            m_navMesh = Chunks::NVNM{aReader};
            break;
        }
    });
}
