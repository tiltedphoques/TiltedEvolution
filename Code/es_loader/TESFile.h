#pragma once

class TESFile
{
    String m_filename;
    Buffer m_buffer;
    uint32_t m_flags;
    uint8_t m_standardId;
    uint16_t m_liteId;

    [[nodiscard]] bool IsLite() const noexcept
    {
        return ((m_flags >> 9) & 1) != 0;
    }

    [[nodiscard]] uint16_t GetId() const noexcept
    {
        return IsLite() ? m_liteId : m_standardId;
    }
};
