
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
        break;
    case 'a':
        return ST_RGBA;
    case 'b':
        return ST_BINARY;
        break;
    case 'c':
        return ST_CHAR;
        break;
    case 'f':
        return ST_FLOAT;
        break;
    case 'h':
        return ST_UCHAR;
        break;
    case 'i':
        return ST_INT;
        break;
    case 'r':
        return ST_RGB;
        break;
    case 'u':
        return ST_UINT;
        break;
    default:
        return ST_NONE;
    }
}
} // namespace BSMain
