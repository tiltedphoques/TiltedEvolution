
#include "REGN.h"
#include <ESLoader.h>

void REGN::ParseChunks(REGN& aSourceRecord, Map<uint8_t, uint32_t>& aParentToFormIdPrefix) noexcept
{
    aSourceRecord.IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) {
        switch (aChunkId)
        {
        case ChunkId::EDID_ID:
            m_editorId = ESLoader::ReadZString(aReader);
            break;
#if 0
        case ChunkId::RDAT_ID: {
            auto it = Chunks::RDAT::RDAT(aReader);
            __debugbreak();
            break;
        }
#endif
        case ChunkId::RDWT_ID: {
            Chunks::RDWT rdwt(aReader);
            m_weatherTypes.push_back(rdwt);
            break;
        }
        }
    });
}
