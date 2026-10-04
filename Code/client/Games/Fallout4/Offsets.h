#pragma once

#include <filesystem>

namespace fo4
{
bool ValidateImage(const std::filesystem::path& acExePath) noexcept;
} // namespace fo4
