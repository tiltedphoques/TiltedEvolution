#pragma once

#include <Records/Group.h>

class Record;
class REFR;
class CLMT;

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

    const Map<uint32_t, REFR*>& GetObjectReferences() const noexcept
    {
        return m_objectReferences;
    }
    const Map<uint32_t, CLMT*>& GetClimates() const noexcept
    {
        return m_climates;
    }

private:
    void BuildFormIdRecordMap() noexcept;
    bool ReadGroupOrRecord(Buffer::Reader& aReader) noexcept;

    String m_filename;
    Buffer m_buffer;
    Map<uint32_t, Record*> m_formIdRecordMap;
    Map<Group*, GroupData> m_groupDataMap;
    uint32_t m_flags;
    union
    {
        uint8_t m_standardId;
        uint16_t m_liteId;
    };

    Map<uint32_t, REFR*> m_objectReferences;
    Map<uint32_t, CLMT*> m_climates;
    Vector<Record*> m_cells;
};
