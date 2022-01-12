#pragma once

class Record
{
    Record() = delete;

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

private:
    uint32_t m_formType;
    uint32_t m_dataSize;
    uint32_t m_flags;
    uint32_t m_formId;
    uint16_t m_timestamp;
    uint16_t m_versionInfo;
    uint16_t m_internalVersion;
    uint16_t m_unk;
};

static_assert(sizeof(Record) == 0x18);

