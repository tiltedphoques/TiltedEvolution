#include "Record.h"

#include <zlib.h>

template <class T> 
void Record::IterateChunks(Map<Record*, SharedPtr<Buffer>>& aCompressedChunkCache, const T& aCallback)
{
    Buffer buffer;
    buffer.Resize(m_dataSize);
    buffer.m_pData = reinterpret_cast<uint8_t*>(this) + sizeof(Record);
    Buffer::Reader reader(&buffer);

    if (Compressed())
    {
        SharedPtr<Buffer>& pDecompressedChunk = aCompressedChunkCache[this];
        if (!pDecompressedChunk)
        {
            uint32_t size = 0;
            reader.ReadBytes(reinterpret_cast<uint8_t*>(&size), 4);

            SharedPtr<Buffer> pDecompressed;
            pDecompressed->Resize(size);
            const uint32_t fieldSize = m_dataSize - 4;

            DecompressChunkData(buffer.GetWriteData() + reader.GetBytePosition(), fieldSize, pDecompressed.GetWriteData(), pDecompressed.GetSize());

            pDecompressedChunk = pDecompressed;
        }

        reader = Buffer::Reader(pDecompressedChunk.get());
    }

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

        Buffer chunkBuffer;
        chunkBuffer.Resize(dataSize);
        Buffer::Reader chunk(&chunkBuffer);

        aCallback(pChunk->m_chunkId, pData, chunk);
    }
}

void Record::DecompressChunkData(const void* apCompressedData, size_t aCompressedSize, void* apDecompressedData, size_t aDecompressedSize)
{
    z_stream compressionStream;
    compressionStream.next_in = (Bytef*)apCompressedData;
    compressionStream.avail_in = (uInt)(aCompressedSize);
    compressionStream.next_out = (Bytef*)apDecompressedData;
    compressionStream.avail_out = (uInt)aDecompressedSize;
    compressionStream.zalloc = Z_NULL;
    compressionStream.zfree = Z_NULL;
    compressionStream.opaque = Z_NULL;

    inflateInit(&compressionStream);
    int res = inflate(&compressionStream, Z_NO_FLUSH);
    if (res < Z_OK)
        spdlog::error("Failed to decompress chunk of data (inflate): {}.", res);
    res = inflateEnd(&compressionStream);
    if (res < Z_OK)
        spdlog::error("Failed to decompress chunk of data: {}", res);
}

