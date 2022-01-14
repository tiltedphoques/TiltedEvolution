#include "Chunks.h"

#include "Record.h"

namespace Chunks
{

PrimaryScripts::PrimaryScripts(const uint8_t* apData, Buffer::Reader& aReader)
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
        script.m_name = String(reinterpret_cast<const char*>(apData + aReader.GetBytePosition()), nameLength);
        aReader.Advance(nameLength);

        aReader.ReadBytes(&script.m_status, 1);
        aReader.ReadBytes(reinterpret_cast<uint8_t*>(&script.m_propertyCount), 2);

        for (uint16_t j; j < script.m_propertyCount; j++)
        {
            ScriptProperty scriptProperty;

            // TODO: ESLoader::ReadZString(Reader&);
            uint32_t propNameLength = 0;
            aReader.ReadBytes(reinterpret_cast<uint8_t*>(&propNameLength), 2);
            scriptProperty.m_name = String(reinterpret_cast<const char*>(apData + aReader.GetBytePosition()), propNameLength);
            aReader.Advance(propNameLength);

            aReader.ReadBytes(reinterpret_cast<uint8_t*>(&scriptProperty.m_type), 1);
            aReader.ReadBytes(reinterpret_cast<uint8_t*>(&scriptProperty.m_status), 1);

            scriptProperty.ParseValue(apData, aReader, m_objectFormat);

            script.m_properties.insert(scriptProperty);
        }
    }
}

void ScriptProperty::ParseValue(const uint8_t* apData, Buffer::Reader& aReader, int16_t aObjectFormat) noexcept
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
        m_dataSingleValue.m_string = { reinterpret_cast<const char*>(apData + aReader.GetBytePosition()), stringLength };
        aReader.Advance(stringLength);
        break;

    case Type::OBJECT_ARRAY:
    case Type::INT_ARRAY:
    case Type::FLOAT_ARRAY:
    case Type::BOOL_ARRAY:
    case Type::STRING_ARRAY:
        uint32_t sizeOfArray = 0;
        aReader.ReadBytes(reinterpret_cast<uint8_t*>(&sizeOfArray), 4);
        for (uint32_t i = 0; i < sizeOfArray; i++)
        {
            ScriptProperty scriptProperty;
            scriptProperty.m_type = GetPropertyType(m_type);
            ParseValue(apData, aReader, aObjectFormat);
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

} // namespace
