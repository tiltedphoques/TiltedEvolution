#pragma once

#include <Records/Group.h>
#include <Records/REFR.h>
#include <Records/CLMT.h>
#include <Records/NPC.h>

class TESFile
{
public:
    TESFile() = default;

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
    [[nodiscard]] uint32_t GetFormIdPrefix() const noexcept
    {
        return m_formIdPrefix;
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

    NPC& GetNpcById(uint32_t aFormId) noexcept
    {
        return m_npcs[aFormId];
    }

private:
    bool ReadGroupOrRecord(Buffer::Reader& aReader) noexcept;

    String m_filename = "";
    Buffer m_buffer{};

    bool m_isLite = false;
    union
    {
        uint8_t m_standardId = 0;
        uint16_t m_liteId;
    };
    uint32_t m_formIdPrefix = 0;

    Map<uint32_t, REFR> m_objectReferences{};
    Map<uint32_t, CLMT> m_climates{};
    Map<uint32_t, NPC> m_npcs{};
};
