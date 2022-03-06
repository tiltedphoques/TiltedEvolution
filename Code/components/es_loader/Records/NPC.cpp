#include "NPC.h"

#include <ESLoader.h>

void NPC::ParseChunks(NPC& aSourceRecord, Map<uint8_t, uint32_t>& aParentToFormIdPrefix) noexcept
{
    aSourceRecord.IterateChunks([&](ChunkId aChunkId, Buffer::Reader& aReader) {
        switch (aChunkId)
        {
        case ChunkId::EDID_ID:
            m_editorId = ESLoader::ReadZString(aReader);
            break;
        case ChunkId::ACBS_ID:
            m_baseStats = Chunks::ACBS(aReader);
            break;
        case ChunkId::DOFT_ID:
            m_defaultOutfit = Chunks::DOFT(aReader, aParentToFormIdPrefix);
            break;
        case ChunkId::VMAD_ID:
            m_scriptData = Chunks::VMAD(aReader, aParentToFormIdPrefix);
            break;
        }
    });
}
