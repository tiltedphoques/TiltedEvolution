#pragma once
// WARNING: !!!! This header musn't include any other project headers !!!!
#include <cstdint>

#ifndef BUILDING_TT_SERVER
#ifdef _WIN32
#define PLUGIN_API extern "C" __declspec(dllexport)
#endif
#endif

class ScriptFunctionContext
{
  protected:
    void* m_pReturn;
    uint32_t m_ArgCount;
    void* m_pArgs;

  public:
    template <typename T> inline T& GetArgument(int idx)
    {
        intptr_t* arguments = (intptr_t*)m_pArgs;

        return *(T*)&arguments[idx];
    }

    template <typename T> inline void SetResult(int idx, T value)
    {
        intptr_t* returnValues = (intptr_t*)m_pReturn;

        if (returnValues)
        {
            *(T*)&returnValues[idx] = value;
        }
    }

    template <typename T> inline T GetResult(int idx)
    {
        intptr_t* returnValues = (intptr_t*)m_pReturn;

        if (returnValues)
        {
            return *(T*)&returnValues[idx];
        }

        return T{};
    }

    inline void* GetArgumentBuffer()
    {
        return m_pArgs;
    }
};

static constexpr uint32_t kPluginMagic = 'PLGN';
static constexpr uint32_t kMaxPluginInterfaceVersion = 1;

// future interfaces shall inherit from this base interface
// e.g PluginInterface002 : PluginInterface001
class PluginInterface001
{
  public:
    virtual ~PluginInterface001() = default;

    virtual uint32_t GetVersion() = 0;

    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;

    enum class EventCode
    {
        kPreHotReload,
        kPreShutdown,
    };
    virtual void OnEvent(EventCode aCode){};
};

struct PluginDescriptor
{
    uint32_t magic;          // < 'PLGN'
    uint32_t pluginVersion;  // < Version of the plugin (user defined)
    const char* pName;       // < Name of the plugin
    const char* pAuthorName; // < Author name

    enum Flags
    {
        kNone = 0,
        kInternal = 1 << 0,  // < reserved for internal "tilted" use
        kHotReload = 1 << 1, // < plugin supports reloading at runtime
    } flags;                 // < Special plugin flags

    // these are for managing the lifetime of the plugin instance on your own heap.
    PluginInterface001* (*CreatePlugin)();      // < allocate from your plugin
    void (*DestroyPlugin)(PluginInterface001*); // < free from your plugin
};
