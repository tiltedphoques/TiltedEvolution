#pragma once

#include <TESFile.h>

namespace fs = std::filesystem;

class ESLoader
{
public:
    struct Plugin
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

    ESLoader() = delete;
    ESLoader(String aDirectory);

    static String LoadZString(Buffer::Reader& aReader) noexcept;

private:
    void FindFiles();
    bool LoadLoadOrder();
    void LoadFiles();

    fs::path GetPath(String& aFilename);

    String m_directory = "";
    Vector<Plugin> m_loadOrder{};
};
