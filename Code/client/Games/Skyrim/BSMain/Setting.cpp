
#include "BSMain/Setting.h"

namespace BSMain
{
Setting::SETTING_TYPE Setting::DataType(const char* apKey)
{
    if (!apKey)
        return ST_NONE;

    switch (*apKey)
    {
    case 'S':
    case 's':
        return ST_STRING;
    case 'a':
        return ST_RGBA;
    case 'b':
        return ST_BINARY;
    case 'c':
        return ST_CHAR;
    case 'f':
        return ST_FLOAT;
    case 'h':
        return ST_UCHAR;
    case 'i':
        return ST_INT;
    case 'r':
        return ST_RGB;
    case 'u':
        return ST_UINT;
    default:
        return ST_NONE;
    }
}
} // namespace BSMain
