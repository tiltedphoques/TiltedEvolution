#pragma once

#include "TESFileRecordTypes.inl"

class Record
{
public:

#pragma pack(push, 1)
    struct Chunk
    {
        ChunkId m_chunkId;
        uint16_t m_dataSize;
    };
#pragma pack(pop)

    enum FLAGS
    {
        kMasterFile = 1,
        kCompressed = 0x40000,
        kIgnored = 0x1000,
        kIsMarker = 0x800000,
    };

    Record() = delete;

    template <class T> 
    void IterateChunks(Map<Record*, SharedPtr<Buffer>>& aCompressedChunkCache, const T& aCallback);

    void DecompressChunkData(const void* apCompressedData, size_t aCompressedSize, void* apDecompressedData, size_t aDecompressedSize);

    [[nodiscard]] FormEnum GetType() const noexcept
    {
        return m_formType;
    }
    [[nodiscard]] uint32_t GetFormId() const noexcept
    {
        return m_formId;
    }
    [[nodiscard]] uint32_t GetDataSize() const noexcept
    {
        return m_dataSize;
    }
    [[nodiscard]] uint32_t GetFlags() const noexcept
    {
        return m_flags;
    }

    [[nodiscard]] bool Compressed() const noexcept
    {
        return (m_flags & FLAGS::kCompressed) != 0;
    }
    [[nodiscard]] bool Ignored() const noexcept
    {
        return (m_flags & FLAGS::kIgnored) != 0;
    }
    [[nodiscard]] bool Master() const noexcept
    {
        return (m_flags & FLAGS::kMasterFile) != 0;
    }

    [[nodiscard]] bool DefaultForm() const noexcept
    {
        return m_formId - 1 <= 0x7FE;
    }

private:
    FormEnum m_formType;
    uint32_t m_dataSize;
    uint32_t m_flags;
    uint32_t m_formId;
    uint32_t m_versionControl;
    uint16_t m_formVersion;
    uint16_t m_vcVersion;
};

static_assert(sizeof(Record) == 0x18);
static_assert(sizeof(Record::Chunk) == 0x6);

