#include "WRLD.h"

#include <ESLoader.h>
#include <iostream>

void WRLD::ParseChunks(WRLD& aSourceRecord, Map<uint8_t, uint32_t>& aParentToFormIdPrefix) noexcept
{
    aSourceRecord.IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) {
        switch (aChunkId)
        {
        case ChunkId::EDID_ID:
            m_editorId = ESLoader::ReadZString(aReader);
            break;
        case ChunkId::WCTR_ID:
            m_centerCell = Chunks::WCTR(aReader);
            break;
        case ChunkId::CNAM_ID:
            aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_climateId), sizeof(m_climateId));
            break;
        case ChunkId::DNAM_ID:
            m_landData = Chunks::DNAM(aReader);
            break;
        case ChunkId::WNAM_ID: {
            uint32_t id = 0;
            aReader.ReadBytes(reinterpret_cast<uint8_t*>(&id), sizeof(id));
            m_parentId = id;
        }
            break;
        case ChunkId::ZNAM_ID:
            aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_musicId), sizeof(m_musicId));
            break;
        case ChunkId::NAMA_ID:
            aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_lodMultiplier), sizeof(m_lodMultiplier));
            break;
        }
    });
}

void WRLD::ParseGRUP() noexcept
{
}
