#include "NPC.h"

#include <ESLoader.h>

void NPC::ParseChunks(NPC& aSourceRecord) noexcept
{
    aSourceRecord.IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) {
        switch (aChunkId)
        {
        case ChunkId::ACBS_ID:
            m_baseStats = Chunks::ACBS(aReader);
            break;
        case ChunkId::DOFT_ID:
            m_defaultOutfit = Chunks::DOFT(aReader);
            break;
        case ChunkId::VMAD_ID:
            m_scriptData = Chunks::VMAD(aReader);
            break;
        }
    });
}
