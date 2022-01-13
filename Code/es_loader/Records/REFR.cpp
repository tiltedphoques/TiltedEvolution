#include "REFR.h"

REFR::Data REFR::ParseChunks() noexcept
{
    Data data;

    IterateChunks([&](ChunkId aChunkId, const uint8_t* apData) { 
        switch (aChunkId)
        {
        case ChunkId::NAME_ID:
            data.m_baseId = *reinterpret_cast<const uint32_t*>(apData);
            break;
        }
    });

    return data;
}
