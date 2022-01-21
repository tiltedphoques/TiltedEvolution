#include "GMST.h"

#include <ESLoader.h>

void GMST::ParseChunks(GMST& aSourceRecord, Map<uint8_t, uint32_t>& aParentToFormIdPrefix) noexcept
{
    aSourceRecord.IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) { 
        switch (aChunkId)
        {
        case ChunkId::EDID_ID:
            m_editorId = ESLoader::ReadZString(aReader);
            break;
        case ChunkId::DATA_ID: {
            if (m_editorId == "")
            {
                spdlog::error("Editor id was not parsed before trying to fetch typed value of game setting.");
                break;
            }
            char valueType = m_editorId[0];
            switch (valueType)
            {
            case 'i':
                m_value.m_type = Chunks::TypedValue::TYPE::INT;
                aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_value.m_int), 4);
                break;
            case 'f':
                m_value.m_type = Chunks::TypedValue::TYPE::FLOAT;
                aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_value.m_float), 4);
                break;
            case 'b':
                m_value.m_type = Chunks::TypedValue::TYPE::BOOL;
                aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_value.m_bool), 4);
                break;
            case 's':
                // TODO: these TypedValue strings are localized
                break;
            }
            break;
        }
        }
    });
}
