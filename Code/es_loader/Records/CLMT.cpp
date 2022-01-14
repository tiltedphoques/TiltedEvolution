#include "CLMT.h"

CLMT::Data CLMT::ParseChunks(Map<Record*, SharedPtr<Buffer>>& aCompressedChunkCache) noexcept
{
    Data data;

    IterateChunks(aCompressedChunkCache, [&](ChunkId aChunkId, const uint8_t* apData) { 
        switch (aChunkId)
        {
        case ChunkId::EDID_ID:
            data.m_editorId = reinterpret_cast<const char*>(apData);
            break;
        case ChunkId::WLST_ID:
            data.m_weatherList = reinterpret_cast<const WeatherList*>(apData);
            break;
        case ChunkId::FNAM_ID:
            data.m_sunTexture = reinterpret_cast<const char*>(apData);
            break;
        case ChunkId::GNAM_ID:
            data.m_glareTexture = reinterpret_cast<const char*>(apData);
            break;
        case ChunkId::TNAM_ID:
            data.m_timing = reinterpret_cast<const SunAndMoon*>(apData);
            break;
        }
    });

    return data;
}
