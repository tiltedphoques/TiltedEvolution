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

    BuildFormIdRecordMap();
}

void TESFile::BuildFormIdRecordMap() noexcept
{
}

template<class T>
Vector<T> TESFile::GetRecords() noexcept
{
    // TODO: I changed TiltedCore locally to expose GetBytePosition() publicly
    Buffer::Reader reader(&m_buffer);
}

