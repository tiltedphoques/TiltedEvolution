#pragma once

namespace Scaleform
{
enum class GFxFileConstants_FileFormatType
{
    File_Unopened = 0x0,
    File_Unknown = 0x1,
    File_SWF = 0x2,
    File_GFX = 0x3,
    File_JPEG = 0xA,
    File_PNG = 0xB,
    File_GIF = 0xC,
    File_TGA = 0xD,
    File_DDS = 0xE,
    File_HDR = 0xF,
    File_BMP = 0x10,
    File_DIB = 0x11,
    File_PFM = 0x12,
    File_TIFF = 0x13,
    File_WAVE = 0x14,
    File_PVR = 0x15,
    File_ETC = 0x16,
    File_SIF = 0x17,
    File_GXT = 0x18,
    File_GTX = 0x19,
    File_GNF = 0x1A,
    File_KTX = 0x1B,
    File_NextAvail = 0x1C,
    File_DDSBC = 0xFFFD,
    File_MultiFormat = 0xFFFE,
    File_Original = 0xFFFF,
};
} // namespace Scaleform
