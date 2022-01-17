#include "Record.h"

#include <zlib.h>

void Record::IterateChunks(const std::function<void(ChunkId, Buffer::Reader&)>& aCallback)
{
    Buffer buffer(reinterpret_cast<uint8_t*>(this) + sizeof(Record), m_dataSize);
    Buffer::Reader reader(&buffer);

    if (Compressed())
    {
        uint32_t size = 0;
        reader.ReadBytes(reinterpret_cast<uint8_t*>(&size), 4);

        Buffer pDecompressed;
        pDecompressed.Resize(size);
        const uint32_t fieldSize = m_dataSize - 4;

        DecompressChunkData(reader.GetDataAtPosition(), fieldSize, pDecompressed.GetWriteData(), pDecompressed.GetSize());

        reader = Buffer::Reader(&pDecompressed);
    }

    uint32_t largeDataSize = 0;

    while (!reader.Eof())
    {
        Record::Chunk* pChunk = reinterpret_cast<Chunk*>(reader.GetDataAtPosition());
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

        reader.Advance(dataSize);

        // TODO: technically, you shouldn't have to copy, in theory, could just leave out the Advance() above
        Buffer::Reader chunk(reader);

        aCallback(pChunk->m_chunkId, chunk);
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

