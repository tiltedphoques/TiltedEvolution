#pragma once

#include "Record.h"

// https://en.uesp.net/wiki/Skyrim_Mod:Mod_File_Format/CLMT
class CLMT : Record
{
public:
    static constexpr FormEnum kType = FormEnum::REFR;

    struct WeatherList // WLST
    {
        uint32_t m_weatherId; // WTHR
        uint32_t m_chance;
        uint32_t m_globalId;
    };

    struct SunAndMoon // TNAM
    {
        uint8_t m_sunriseBegin; // times 10 minutes
        uint8_t m_sunriseEnd; // times 10 minutes
        uint8_t m_sunsetBegin; // times 10 minutes
        uint8_t m_sunsetEnd; // times 10 minutes
        uint8_t m_volatility; // 0-100
        uint8_t m_moons;
    };

    struct Data
    {
        // EDID
        const char* m_editorId = "";
        // WLST
        const WeatherList* m_weatherList = nullptr;
        // FNAM
        const char* m_sunTexture = "";
        // GNAM
        const char* m_glareTexture = "";
        //TNAM
        const SunAndMoon* m_timing = nullptr;
    };

    Data ParseChunks(Map<Record*, SharedPtr<Buffer>>& aCompressedChunkCache) noexcept;
};
