#include "TESFile.h"

#include <filesystem>
#include <fstream>

#include "Records/Group.h"

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
    // TODO: I changed TiltedCore locally to expose GetBytePosition() publicly
    Buffer::Reader reader(&m_buffer);

    while (!reader.Eof())
    {
        uint32_t type = Serialization::ReadVarInt(reader) & 0xFFFFFFFF;
        uint32_t size = Serialization::ReadVarInt(reader) & 0xFFFFFFFF;

        if (type == 0x50555247) // GRUP
        {
            const Group* pGroup = reinterpret_cast<const Group*>(m_buffer.GetData() + reader.GetBytePosition());
        }
        else
        {
        
        }
    }
}

template<class T>
Vector<T> TESFile::GetRecords() noexcept
{
    // TODO: I changed TiltedCore locally to expose GetBytePosition() publicly
    Buffer::Reader reader(&m_buffer);
}

