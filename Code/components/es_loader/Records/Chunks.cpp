#include "Chunks.h"

#include "Record.h"
#include <ESLoader.h>

namespace Chunks
{

uint32_t ReadFormId(Buffer::Reader& aReader, Map<uint8_t, uint32_t>& aParentToFormIdPrefix)
{
    uint32_t formId = 0;
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&formId), 4);

    uint32_t realBaseId = TESFile::GetFormIdPrefix(formId, aParentToFormIdPrefix);

    formId &= 0x00FFFFFF;
    formId += realBaseId;

    return formId;
}

VMAD::VMAD(Buffer::Reader& aReader, Map<uint8_t, uint32_t>& aParentToFormIdPrefix)
{
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_version), 2);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_objectFormat), 2);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_scriptCount), 2);
    
    m_scripts.reserve(m_scriptCount);

    for (uint16_t i = 0; i < m_scriptCount; i++)
    {
        Script script;

        script.m_name = ESLoader::ReadWString(aReader);

        aReader.ReadBytes(&script.m_status, 1);
        aReader.ReadBytes(reinterpret_cast<uint8_t*>(&script.m_propertyCount), 2);

        for (uint16_t j = 0; j < script.m_propertyCount; j++)
        {
            ScriptProperty scriptProperty;

            scriptProperty.m_name = ESLoader::ReadWString(aReader);

            aReader.ReadBytes(reinterpret_cast<uint8_t*>(&scriptProperty.m_type), 1);
            aReader.ReadBytes(reinterpret_cast<uint8_t*>(&scriptProperty.m_status), 1);

            scriptProperty.ParseValue(aReader, m_objectFormat, aParentToFormIdPrefix);

            script.m_properties.push_back(scriptProperty);
        }

        m_scripts.push_back(script);
    }
}

void ScriptProperty::ParseValue(Buffer::Reader& aReader, int16_t aObjectFormat, Map<uint8_t, uint32_t>& aParentToFormIdPrefix) noexcept
{
    switch (m_type)
    {
    case Type::OBJECT:
        if (aObjectFormat == 1)
        {
            m_dataSingleValue.m_formId = ReadFormId(aReader, aParentToFormIdPrefix);
        }
        else if (aObjectFormat == 2)
        {
            aReader.Advance(4);
            m_dataSingleValue.m_formId = ReadFormId(aReader, aParentToFormIdPrefix);
        }
        break;

    case Type::INT:
        aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_dataSingleValue.m_integer), 4);
        break;
    case Type::FLOAT:
        aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_dataSingleValue.m_float), 4);
        break;
    case Type::BOOL:
        aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_dataSingleValue.m_boolean), 1);
        break;

    case Type::WSTRING: {
        uint32_t stringLength = 0;
        aReader.ReadBytes(reinterpret_cast<uint8_t*>(&stringLength), 2);
        m_dataSingleValue.m_string = { reinterpret_cast<const char*>(aReader.GetDataAtPosition()), stringLength };
        aReader.Advance(stringLength);
        break;
    }

    case Type::OBJECT_ARRAY:
    case Type::INT_ARRAY:
    case Type::FLOAT_ARRAY:
    case Type::BOOL_ARRAY:
    case Type::STRING_ARRAY: {
        uint32_t sizeOfArray = 0;
        aReader.ReadBytes(reinterpret_cast<uint8_t*>(&sizeOfArray), 4);
        for (uint32_t i = 0; i < sizeOfArray; i++)
        {
            ScriptProperty scriptProperty;
            scriptProperty.m_type = GetPropertyType(m_type);
            ParseValue(aReader, aObjectFormat, aParentToFormIdPrefix);
            m_dataArray.push_back(scriptProperty.m_dataSingleValue);
        }

        break;
    }
    }
}

ScriptProperty::Type ScriptProperty::GetPropertyType(Type aArrayType) noexcept
{
    return static_cast<Type>(static_cast<int>(aArrayType) - 10);
}

CNTO::CNTO(Buffer::Reader& aReader)
{
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_formId), 4);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_count), 4);
}

WLST::WLST(Buffer::Reader& aReader, Map<uint8_t, uint32_t>& aParentToFormIdPrefix)
{
    m_weatherId = ReadFormId(aReader, aParentToFormIdPrefix);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_chance), 4);
    m_globalId = ReadFormId(aReader, aParentToFormIdPrefix);
}

TNAM::TNAM(Buffer::Reader& aReader)
{
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_sunriseBegin), 1);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_sunriseEnd), 1);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_sunsetBegin), 1);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_sunsetEnd), 1);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_volatility), 1);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_moons), 1);
}

NAME::NAME(Buffer::Reader& aReader, Map<uint8_t, uint32_t>& aParentToFormIdPrefix)
{
    m_baseId = ReadFormId(aReader, aParentToFormIdPrefix);
}

DOFT::DOFT(Buffer::Reader& aReader, Map<uint8_t, uint32_t>& aParentToFormIdPrefix)
{
    m_formId = ReadFormId(aReader, aParentToFormIdPrefix);
}

ACBS::ACBS(Buffer::Reader& aReader)
{
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_flags), 4);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_magickaOffset), 2);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_staminaOffset), 2);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_level), 2);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_calcMinLevel), 2);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_calcMaxLevel), 2);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_speedMultiplier), 2);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_dispositionBase), 2);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_templateDataFlags), 2);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_healthOffset), 2);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_bleedoutOverride), 2);
}

MAST::MAST(Buffer::Reader& aReader)
{
    m_masterName = ESLoader::ReadZString(aReader);
}

WCTR::WCTR(Buffer::Reader& aReader)
{
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_x), 2);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_y), 2);
}

DNAM::DNAM(Buffer::Reader& aReader)
{
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_landLevel), 4);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_waterLevel), 4);
}

} // namespace
