#pragma once

#include <TESFile.h>

namespace fs = std::filesystem;

namespace ESLoader
{
struct RecordCollection;

struct PluginData
{
    [[nodiscard]] bool IsLite() const noexcept
    {
        return m_isLite;
    }

    String m_filename;
    union {
        uint8_t m_standardId;
        uint16_t m_liteId;
    };
    bool m_isLite;
};
using PluginCollection = Vector<PluginData>;

String ReadZString(Buffer::Reader& aReader) noexcept;
String ReadWString(Buffer::Reader& aReader) noexcept;

class ESLoader
{
  public:
    ESLoader();

    UniquePtr<RecordCollection> BuildRecordCollection() noexcept;

    PluginCollection& GetLoadOrder() noexcept
    {
        return m_loadOrder;
    }

  private:
    bool LoadLoadOrder();
    UniquePtr<RecordCollection> LoadFiles();

    fs::path GetPath(String& aFilename);

    fs::path m_directory = "";
    Vector<PluginData> m_loadOrder{};
    TiltedPhoques::Map<String, uint8_t> m_masterFiles{};
};
} // namespace ESLoader
