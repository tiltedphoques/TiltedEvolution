#pragma once

class Record
{
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
    uint32_t m_formId;
    uint32_t m_dataSize;
    uint32_t m_flags;
};
