#include "CONT.h"

#include <ESLoader.h>

CONT::Data CONT::ParseChunks() noexcept
{
    Data data;

    IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) {
        switch (aChunkId)
        {
        case ChunkId::EDID_ID:
            data.m_editorId = ESLoader::LoadZString(aReader);
            break;
        case ChunkId::FULL_ID:
            data.m_name = ESLoader::LoadZString(aReader);
            break;
        case ChunkId::CNTO_ID:
            Chunks::CNTO cnto(aReader);
            data.m_objects.push_back(cnto);
            break;
        }
    });

    return data;
}
