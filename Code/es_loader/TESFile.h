#pragma once

class Record;

class TESFile
{
public:
    TESFile() = delete;
    TESFile(const std::filesystem::path& acPath);

    [[nodiscard]] bool IsLite() const noexcept
    {
        return ((m_flags >> 9) & 1) != 0;
    }
    [[nodiscard]] uint16_t GetId() const noexcept
    {
        return IsLite() ? m_liteId : m_standardId;
    }
    [[nodiscard]] String GetFilename() const noexcept
    {
        return m_filename;
    }

    template<class T> Vector<T> GetRecords() noexcept;

private:
    void BuildFormIdRecordMap() noexcept;

    String m_filename;
    Buffer m_buffer;
    Map<uint32_t, Record*> m_formIdRecordMap;
    uint32_t m_flags;
    union
    {
        uint8_t m_standardId;
        uint16_t m_liteId;
    };
};
