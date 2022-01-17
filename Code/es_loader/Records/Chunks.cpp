#include "Chunks.h"

#include "Record.h"

namespace Chunks
{

PrimaryScripts::PrimaryScripts(Buffer::Reader& aReader)
{
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_version), 2);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_objectFormat), 2);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_scriptCount), 2);
    
    m_scripts.resize(m_scriptCount);

    for (uint16_t i = 0; i < m_scriptCount; i++)
    {
        Script script;

        // TODO: ESLoader::ReadZString(Reader&);
        uint32_t nameLength = 0;
        aReader.ReadBytes(reinterpret_cast<uint8_t*>(&nameLength), 2);
        script.m_name = String(reinterpret_cast<const char*>(aReader.GetDataAtPosition()), nameLength);
        aReader.Advance(nameLength);

        aReader.ReadBytes(&script.m_status, 1);
        aReader.ReadBytes(reinterpret_cast<uint8_t*>(&script.m_propertyCount), 2);

        for (uint16_t j = 0; j < script.m_propertyCount; j++)
        {
            ScriptProperty scriptProperty;

            // TODO: ESLoader::ReadZString(Reader&);
            uint32_t propNameLength = 0;
            aReader.ReadBytes(reinterpret_cast<uint8_t*>(&propNameLength), 2);
            scriptProperty.m_name = String(reinterpret_cast<const char*>(aReader.GetDataAtPosition()), propNameLength);
            aReader.Advance(propNameLength);

            aReader.ReadBytes(reinterpret_cast<uint8_t*>(&scriptProperty.m_type), 1);
            aReader.ReadBytes(reinterpret_cast<uint8_t*>(&scriptProperty.m_status), 1);

            scriptProperty.ParseValue(aReader, m_objectFormat);

            script.m_properties.push_back(scriptProperty);
        }
    }
}

void ScriptProperty::ParseValue(Buffer::Reader& aReader, int16_t aObjectFormat) noexcept
{
    switch (m_type)
    {
    case Type::OBJECT:
        if (aObjectFormat == 1)
            aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_dataSingleValue.m_formId), 4);
        else if (aObjectFormat == 2)
        {
            aReader.Advance(4);
            aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_dataSingleValue.m_formId), 4);
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
            ParseValue(aReader, aObjectFormat);
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

WLST::WLST(Buffer::Reader& aReader)
{
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_weatherId), 4);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_chance), 4);
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_globalId), 4);
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

NAME::NAME(Buffer::Reader& aReader)
{
    aReader.ReadBytes(reinterpret_cast<uint8_t*>(&m_baseId), 4);
}

} // namespace
