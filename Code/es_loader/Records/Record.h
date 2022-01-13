#pragma once

enum class FormEnum : uint32_t
{
    REFR = 0x52464552,
    ACHR = 0x52484341,
};

class Record
{
public:
    Record() = delete;

    [[nodiscard]] FormEnum GetType() const noexcept
    {
        return m_formType;
    }
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
    FormEnum m_formType;
    uint32_t m_dataSize;
    uint32_t m_flags;
    uint32_t m_formId;
    uint32_t m_versionControl;
    uint16_t m_formVersion;
    uint16_t m_vcVersion;
};

static_assert(sizeof(Record) == 0x18);

