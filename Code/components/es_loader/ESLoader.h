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

    ESLoader();

    UniquePtr<RecordCollection> BuildRecordCollection() noexcept;

private:
    bool LoadLoadOrder();
    UniquePtr<RecordCollection> LoadFiles();

    fs::path GetPath(String& aFilename);

    String m_directory = "";
    Vector<PluginData> m_loadOrder{};
    Map<String, uint8_t> m_masterFiles{};
};
