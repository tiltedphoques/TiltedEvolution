#include <cstdint>

extern "C" __declspec(dllimport) int ScriptExtenderFixtureDependency();

namespace
{
int s_startCount = 0;
}

extern "C"
{
    __declspec(dllexport) int GetFixtureStartCount()
    {
        return s_startCount;
    }

    __declspec(dllexport) int GetFixtureDependencyValue()
    {
        return ScriptExtenderFixtureDependency();
    }

#if !defined(SCRIPT_EXTENDER_FIXTURE_NO_ENTRYPOINT)
#if defined(TP_FALLOUT4)
    __declspec(dllexport) void StartF4SE()
#else
    __declspec(dllexport) void StartSKSE()
#endif
    {
        ++s_startCount;
    }
#endif

#if defined(TP_FALLOUT4)
    struct CoreVersion
    {
        uint32_t DataVersion;
        uint32_t RuntimeVersion;
    };

    __declspec(dllexport) CoreVersion F4SECore_Version{
#if defined(SCRIPT_EXTENDER_FIXTURE_BAD_SCHEMA)
        2,
#else
        1,
#endif
#if defined(SCRIPT_EXTENDER_FIXTURE_BAD_RUNTIME)
        0x010A3D80 // Fallout 4 1.10.984, rather than 1.11.240.
#else
        0x010B0F00
#endif
    };
#endif
}
