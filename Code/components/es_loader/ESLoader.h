#pragma once

#include <TESFile.h>

namespace fs = std::filesystem;

class RecordCollection;

class ESLoader
{
public:
    struct PluginData
    {
        [[nodiscard]] bool IsLite() const noexcept
        {
            return m_isLite;
        }

        String m_filename;
        union
        {
            uint8_t m_standardId;
            uint16_t m_liteId;
        };
        bool m_isLite;
    };

    static String ReadZString(Buffer::Reader& aReader) noexcept;
    static String ReadWString(Buffer::Reader& aReader) noexcept;

    ESLoader() = delete;
    ESLoader(String aDirectory);

    RecordCollection BuildRecordCollection() noexcept;

private:
    void FindFiles();
    bool LoadLoadOrder();
    RecordCollection LoadFiles();

    fs::path GetPath(String& aFilename);

    String m_directory = "";
    Vector<PluginData> m_loadOrder{};
    Map<String, uint8_t> m_masterFiles{};
};
