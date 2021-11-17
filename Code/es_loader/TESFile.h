#pragma once

class TESFile
{
public:
    [[nodiscard]] bool IsLite() const noexcept
    {
        return ((m_flags >> 9) & 1) != 0;
    }

    [[nodiscard]] uint16_t GetId() const noexcept
    {
        return IsLite() ? m_liteId : m_standardId;
    }

private:
    String m_filename;
    Buffer m_buffer;
    uint32_t m_flags;
    union
    {
        uint8_t m_standardId;
        uint16_t m_liteId;
    };
};
