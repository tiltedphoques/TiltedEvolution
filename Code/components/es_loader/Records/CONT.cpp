#include "CONT.h"

#include <ESLoader.h>

void CONT::ParseChunks(CONT& aSourceRecord, Map<uint8_t, uint32_t>& aParentToFormIdPrefix) noexcept
{
    aSourceRecord.IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) {
        switch (aChunkId)
        {
        case ChunkId::EDID_ID:
            m_editorId = ESLoader::ReadZString(aReader);
            break;
        case ChunkId::FULL_ID:
            // TODO: FULL has lstring (localized), need to extract it
            //aParsedRecord.m_name = ESLoader::ReadZString(aReader);
            break;
        case ChunkId::CNTO_ID:
            Chunks::CNTO cnto(aReader);
            m_objects.push_back(cnto);
            break;
        }
    });
}
