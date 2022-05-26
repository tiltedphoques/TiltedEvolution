#pragma once

#include <RecordCollection.h>

#include <Records/CLMT.h>
#include <Records/GMST.h>
#include <Records/Group.h>
#include <Records/NPC.h>
#include <Records/REFR.h>
#include <Records/TES4.h>

namespace ESLoader
{
class TESFile
{
  public:
    TESFile() = default;
    TESFile(TiltedPhoques::Map<String, uint8_t> &aMasterFiles);

    void Setup(uint8_t aStandardId);
    void Setup(uint16_t aLiteId);
    bool LoadFile(const std::filesystem::path& acPath) noexcept;
    bool IndexRecords(RecordCollection& aRecordCollection) noexcept;

    [[nodiscard]] static uint32_t GetFormIdPrefix(uint32_t aFormId, TiltedPhoques::Map<uint8_t, uint32_t>& aParentToFormIdPrefix) noexcept;

  private:
    bool ReadGroupOrRecord(Buffer::Reader& aReader, RecordCollection& aRecordCollection) noexcept;

    template <class T> T CopyAndParseRecord(Record* pRecordHeader);

    template <class T> void ParseGRUP(Record* pRecordHeader, T& aRecord);

    String m_filename = "";
    Buffer m_buffer{};

    union {
        uint8_t m_standardId = 0;
        uint16_t m_liteId;
    };
    uint32_t m_formIdPrefix = 0;

    TiltedPhoques::Map<String, uint8_t> &m_masterFiles;
    TiltedPhoques::Map<uint8_t, uint32_t> m_parentToFormIdPrefix{};
};

} // namespace ESLoader
