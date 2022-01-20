#include "TES4.h"

#include <ESLoader.h>

TES4 TES4::ParseChunks() noexcept
{
    TES4 tes4;
    tes4.CopyRecordData(*this);

    IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) {
        switch (aChunkId)
        {
        case ChunkId::MAST_ID:
            Chunks::MAST mast(aReader);
            tes4.m_masterFiles.push_back(mast);
            break;
        }
    });

    return tes4;
}
