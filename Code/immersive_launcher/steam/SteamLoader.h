#pragma once

#include <TiltedCore/Filesystem.hpp>

namespace steam
{
namespace fs = std::filesystem;

void Load(const fs::path& aGameDir);
} // namespace steam
