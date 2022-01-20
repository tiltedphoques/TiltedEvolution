#include "TESFile.h"

#include <filesystem>
#include <fstream>

TESFile::TESFile(Map<String, uint8_t> aMasterFiles)
    : m_masterFiles(aMasterFiles)
{
}

void TESFile::Setup(uint8_t aStandardId)
{
    m_standardId = aStandardId;
    m_formIdPrefix = m_standardId * 0x1000000;
}

void TESFile::Setup(uint16_t aLiteId)
{
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

bool TESFile::IndexRecords(RecordCollection& aRecordCollection) noexcept
{
    if (m_filename.size() == 0)
        return false;

    Buffer::Reader reader(&m_buffer);

    while (true)
    {
        if (!ReadGroupOrRecord(reader, aRecordCollection))
            break;
    }

    return true;
}

bool TESFile::ReadGroupOrRecord(Buffer::Reader& aReader, RecordCollection& aRecordCollection) noexcept
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
        const size_t endOfGroup = aReader.GetBytePosition() + size;
        aReader.Advance(sizeof(Group));

        while (aReader.GetBytePosition() < endOfGroup)
        {
            ReadGroupOrRecord(aReader, aRecordCollection);
        }
    }
    else // Records
    {
        Record* pRecord = reinterpret_cast<Record*>(m_buffer.GetWriteData() + aReader.GetBytePosition());

        switch (pRecord->GetType())
        {
        case FormEnum::TES4: {
            TES4 fileHeader = CopyAndParseRecord<TES4>(pRecord);

            uint8_t parentId = 0;
            for (const Chunks::MAST& master : fileHeader.m_masterFiles)
            {
                m_parentToMaster[parentId] = m_masterFiles[master.m_masterName];
                parentId++;
            }

            break;
        }
        //case FormEnum::ACHR:
        case FormEnum::REFR: {
            REFR parsedRecord = CopyAndParseRecord<REFR>(pRecord);
            aRecordCollection.m_objectReferences[parsedRecord.GetFormId()] = parsedRecord;
            break;
        }
        case FormEnum::CELL:
            break;
        case FormEnum::CLMT: {
            CLMT parsedRecord = CopyAndParseRecord<CLMT>(pRecord);
            aRecordCollection.m_climates[parsedRecord.GetFormId()] = parsedRecord;
            break;
        }
        case FormEnum::NPC_: {
            NPC parsedRecord = CopyAndParseRecord<NPC>(pRecord);
            aRecordCollection.m_npcs[parsedRecord.GetFormId()] = parsedRecord;
            break;
        }
        case FormEnum::CONT: {
            CONT parsedRecord = CopyAndParseRecord<CONT>(pRecord);
            aRecordCollection.m_containers[parsedRecord.GetFormId()] = parsedRecord;
            break;
        }
        }

        //pRecord->DiscoverChunks();

        Record record;
        record.CopyRecordData(*pRecord);
        uint8_t baseId = (uint8_t)(pRecord->GetFormId() >> 24);
        record.SetBaseId(GetFormIdPrefix(baseId));
        aRecordCollection.m_allRecords[pRecord->GetFormId()] = *pRecord;

        aReader.Advance(sizeof(Record) + size);
    }

    return true;
}

template <class T> 
T TESFile::CopyAndParseRecord(Record* pRecordHeader)
{
    T* pRecord = reinterpret_cast<T*>(pRecordHeader);

    T parsedRecord;
    parsedRecord.CopyRecordData(*pRecord);
    parsedRecord.ParseChunks(*pRecord);

    uint8_t baseId = (uint8_t)(pRecord->GetFormId() >> 24);
    parsedRecord.SetBaseId(GetFormIdPrefix(baseId));

    return parsedRecord;
}

uint32_t TESFile::GetFormIdPrefix(uint8_t aCurrentPrefix) const noexcept
{
    auto masterId = m_parentToMaster.find(aCurrentPrefix);
    if (masterId != std::end(m_parentToMaster))
        return ((uint32_t)masterId->second) << 24;
    else
        return m_formIdPrefix;
}
