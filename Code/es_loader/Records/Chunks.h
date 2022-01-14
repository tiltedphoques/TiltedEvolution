#pragma once

class Record;

namespace Chunks
{

struct ScriptProperty
{
    enum class Type
    {
        INVALID = 0,
        OBJECT = 1,
        WSTRING = 2,
        INT = 3,
        FLOAT = 4,
        BOOL = 5,
        OBJECT_ARRAY = 11,
        STRING_ARRAY = 12,
        INT_ARRAY = 13,
        FLOAT_ARRAY = 14,
        BOOL_ARRAY = 15
    };

    enum class Status
    {
        PROPERTY_EDITED = 1,
        PROPERTY_REMOVED = 3,
    };

    union Data {
        uint32_t m_formId;
        uint32_t m_integer;
        float m_float;
        bool m_boolean;

        struct Str
        {
          const char* data;
          size_t length;
        } m_string {nullptr, 0};
    };

    void ParseValue(Buffer::Reader& aReader) noexcept;

    String m_name;
    Type m_type = Type::INVALID;
    Status m_status = Status::PROPERTY_EDITED;
    Data m_dataSingleValue;
    Vector<Data> m_dataArray;
};

struct Script
{
    String m_name;
    uint8_t m_status;
    uint16_t m_propertyCount;
    Set<ScriptProperty> m_properties;
};

struct PrimaryScripts
{
    PrimaryScripts(const uint8_t* apData, Buffer::Reader& aReader);

    int16_t m_version = 0;
    int16_t m_objectFormat = 0;
    uint16_t m_scriptCount = 0;
    Vector<Script> m_scripts;
};

} // namespace
