#include "TESFile.h"

#include <filesystem>
#include <fstream>

TESFile::TESFile(const std::filesystem::path& acPath)
{
    m_filename = acPath.filename().string();

    const uintmax_t fileSize = std::filesystem::file_size(acPath);
    m_buffer.Resize(fileSize);

    std::ifstream file(acPath, std::ios::binary);
    file.read(reinterpret_cast<char*>(m_buffer.GetWriteData()), fileSize);
}

