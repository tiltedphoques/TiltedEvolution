#pragma once

#include <Records/Group.h>
#include <Records/REFR.h>
#include <Records/CLMT.h>
#include <Records/NPC.h>
#include <Records/TES4.h>

class TESFile
{
public:
    TESFile() = default;
    TESFile(Map<String, uint8_t> aMasterFiles);

    void Setup(uint8_t aStandardId);
    void Setup(uint16_t aLiteId);
    bool LoadFile(const std::filesystem::path& acPath) noexcept;
    bool IndexRecords() noexcept;

    [[nodiscard]] bool IsLite() const noexcept
    {
        return m_isLite;
    }
    [[nodiscard]] uint16_t GetId() const noexcept
    {
        return IsLite() ? m_liteId : m_standardId;
    }
    [[nodiscard]] String GetFilename() const noexcept
    {
        return m_filename;
    }

    const Map<uint32_t, REFR>& GetObjectReferences() const noexcept
    {
        return m_objectReferences;
    }
    const Map<uint32_t, CLMT>& GetClimates() const noexcept
    {
        return m_climates;
    }
    const Map<uint32_t, NPC>& GetNPCs() const noexcept
    {
        return m_npcs;
    }

    NPC& GetNpcById(uint32_t aFormId) noexcept
    {
        return m_npcs[aFormId];
    }

    [[nodiscard]] uint32_t GetFormIdPrefix(uint8_t aCurrentPrefix) const noexcept;

private:
    bool ReadGroupOrRecord(Buffer::Reader& aReader) noexcept;

    template <class T> 
    T CopyAndParseRecord(Record* pRecordHeader);

    String m_filename = "";
    Buffer m_buffer{};

    bool m_isLite = false;
    union
    {
        uint8_t m_standardId = 0;
        uint16_t m_liteId;
    };
    uint32_t m_formIdPrefix = 0;

    Map<String, uint8_t> m_masterFiles{};
    Map<uint8_t, uint8_t> m_parentToMaster{};

    Map<uint32_t, REFR> m_objectReferences{};
    Map<uint32_t, CLMT> m_climates{};
    Map<uint32_t, NPC> m_npcs{};
};
