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

            scriptProperty.ParseValue(aReader);

            script.m_properties.insert(scriptProperty);
        }
    }
}

void ScriptProperty::ParseValue(Buffer::Reader& aReader) noexcept
{

}

} // namespace
