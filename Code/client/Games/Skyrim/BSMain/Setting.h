#pragma once

namespace BSMain
{
class Setting
{
  public:
    enum SETTING_TYPE : int
    {
        ST_BINARY = 0x0,
        ST_CHAR = 0x1,
        ST_UCHAR = 0x2,
        ST_INT = 0x3,
        ST_UINT = 0x4,
        ST_FLOAT = 0x5,
        ST_STRING = 0x6,
        ST_RGB = 0x7,
        ST_RGBA = 0x8,
        ST_NONE = 0x9,
    };

    virtual ~Setting() = default;

    // Is it a member of the user customizeable collection?
    virtual bool IsPrefSetting()
    {
        return false;
    }

    static SETTING_TYPE DataType(const char* apKey);

    const char* GetName() const
    {
        return pKey;
    }

    // To data type
    int asInt() const
    {
        return value.i;
    }

    float AsFloat() const
    {
        return value.f;
    }

    bool asBool() const
    {
        return value.b;
    }

  public:
    union {
        float f;
        int i;
        uint32_t u;
        bool b;
        char c;
        const char* str;
    } value;
    const char* pKey;
};

static_assert(sizeof(Setting) == 0x18);
} // namespace BSMain
