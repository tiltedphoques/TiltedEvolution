#include "TES4.h"

#include <ESLoader.h>

void TES4::ParseChunks(TES4& aSourceRecord) noexcept
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
