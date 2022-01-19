#include "TESFile.h"

#include <filesystem>
#include <fstream>

#include "Records/Record.h"
#include "Records/REFR.h"
#include "Records/CLMT.h"
#include "Records/NPC.h"

void TESFile::Setup(uint8_t aStandardId)
{
    m_isLite = false;
    m_standardId = aStandardId;
    m_formIdPrefix = m_standardId * 0x1000000;
}

void TESFile::Setup(uint16_t aLiteId)
{
    m_isLite = true;
    m_liteId = aLiteId;
    m_formIdPrefix = 0xFE000000 + (m_liteId * 0x1000);
}

bool TESFile::LoadFile(const std::filesystem::path& acPath) noexcept
{
    m_filename = acPath.filename().string();

    const uintmax_t fileSize = std::filesystem::file_size(acPath);
    m_buffer.Resize(fileSize);

    std::ifstream file(acPath, std::ios::binary);
    if (file.fail())
    {
        spdlog::error("Failed to open plugin {}", m_filename);
        return false;
    }

    file.read(reinterpret_cast<char*>(m_buffer.GetWriteData()), fileSize);
    return true;
}

bool TESFile::IndexRecords() noexcept
{
    if (m_filename.size() == 0)
        return false;

    Buffer::Reader reader(&m_buffer);

    while (true)
    {
        if (!ReadGroupOrRecord(reader))
            break;
    }

    return true;
}

bool TESFile::ReadGroupOrRecord(Buffer::Reader& aReader) noexcept
{
    if (aReader.Eof())
        return false;

    uint32_t type = 0;
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&type), 4);
    uint32_t size = 0;
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&size), 4);
    aReader.Reverse(8);

    if (type == static_cast<uint32_t>(FormEnum::GRUP))
    {
        aReader.Advance(sizeof(Group));
        const size_t endOfGroup = aReader.GetBytePosition() + size - 0x18;

        while (aReader.GetBytePosition() < endOfGroup)
        {
            ReadGroupOrRecord(aReader);
        }
    }
    else // Records
    {
        Record* pRecord = reinterpret_cast<Record*>(m_buffer.GetWriteData() + aReader.GetBytePosition());

        switch (pRecord->GetType())
        {
        //case FormEnum::ACHR:
        case FormEnum::REFR: {
            REFR recordREFR;
            recordREFR.CopyRecordData(*pRecord);
            recordREFR.SetBaseId(GetFormIdPrefix());
            recordREFR.ParseChunks();
            m_objectReferences[recordREFR.GetFormId()] = recordREFR;
            break;
        }
        case FormEnum::CELL:
            break;
        case FormEnum::CLMT: {
            CLMT recordCLMT;
            recordCLMT.CopyRecordData(*pRecord);
            recordCLMT.SetBaseId(GetFormIdPrefix());
            recordCLMT.ParseChunks();
            m_climates[pRecord->GetFormId()] = recordCLMT;
            break;
        }
        case FormEnum::NPC_: {
            NPC recordNPC;
            recordNPC.CopyRecordData(*pRecord);
            recordNPC.SetBaseId(GetFormIdPrefix());
            recordNPC.ParseChunks();
            m_npcs[pRecord->GetFormId()] = recordNPC;
            break;
        }
        }

        aReader.Advance(sizeof(Record) + size);
    }

    return true;
}

