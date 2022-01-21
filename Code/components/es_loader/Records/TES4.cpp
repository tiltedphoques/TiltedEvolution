#include "TES4.h"

#include <ESLoader.h>

void TES4::ParseChunks(TES4& aSourceRecord, Map<uint8_t, uint32_t>& aParentToFormIdPrefix) noexcept
{
    aSourceRecord.IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) {
        switch (aChunkId)
        {
        case ChunkId::MAST_ID:
            Chunks::MAST mast(aReader);
            m_masterFiles.push_back(mast);
            break;
        }
    });
}
