#include "Record.h"

template <class T> 
void Record::IterateChunks(const T& aCallback)
{
    Buffer buffer;
    buffer.Resize(m_dataSize);
    buffer.m_pData = reinterpret_cast<uint8_t*>(this) + sizeof(Record);
    Buffer::Reader reader(&buffer);

    uint32_t largeDataSize = 0;

    while (!reader.Eof())
    {
        Record::Chunk* pChunk = reinterpret_cast<Chunk*>(buffer.GetWriteData() + reader.GetBytePosition());
        reader.Advance(sizeof(Record::Chunk));

        uint32_t dataSize = pChunk->m_dataSize;
        if (pChunk->m_dataSize == 0)
        {
            dataSize = largeDataSize;
        }

        // Chunk XXXX will have the next data size stored at the start of the field
        // https://en.uesp.net/wiki/Skyrim_Mod:Mod_File_Format#Fields
        if (pChunk->m_chunkId == ChunkId::XXXX_ID)
        {
            reader.ReadBytes(reinterpret_cast<uint8_t*>(&largeDataSize), 4);
            reader.Reverse(4);
        }

        const uint8_t* pData = buffer.GetData() + reader.GetBytePosition();
        reader.Advance(dataSize);

        aCallback(pChunk->m_chunkId, pData);
    }
}

