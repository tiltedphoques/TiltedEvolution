#include "CONT.h"

#include <ESLoader.h>

CONT CONT::ParseChunks() noexcept
{
    CONT cont;
    cont.CopyRecordData(*this);

    IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) {
        switch (aChunkId)
        {
        case ChunkId::EDID_ID:
            cont.m_editorId = ESLoader::ReadZString(aReader);
            break;
        case ChunkId::FULL_ID:
            // TODO: FULL has lstring (localized), need to extract it
            //cont.m_name = ESLoader::ReadZString(aReader);
            break;
        case ChunkId::CNTO_ID:
            Chunks::CNTO cnto(aReader);
            cont.m_objects.push_back(cnto);
            break;
        }
    });

    return cont;
}
