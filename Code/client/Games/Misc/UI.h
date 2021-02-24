#pragma once

struct BSFixedString;

struct UI
{
    [[nodiscard]] static UI* Get() noexcept;

    [[nodiscard]] bool IsOpen(const BSFixedString& acName) const noexcept;
};
