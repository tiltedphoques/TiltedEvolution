#include "CONT.h"

CONT::Data CONT::ParseChunks(Map<Record*, SharedPtr<Buffer>>& aCompressedChunkCache) noexcept
{
    Data data;

    IterateChunks(aCompressedChunkCache, [&](ChunkId aChunkId, const uint8_t* apData) { 
        switch (aChunkId)
        {
        case ChunkId::EDID_ID:
            data.m_editorId = reinterpret_cast<const char*>(apData);
            break;
        case ChunkId::FULL_ID:
            data.m_name = reinterpret_cast<const char*>(apData);
            break;
        case ChunkId::CNTO_ID:
            data.m_objects.push_back(*reinterpret_cast<const CONT::Object*>(apData));
            break;
        }
    });

    return data;
}
