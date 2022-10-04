#pragma once
// WARNING: !!!! This header musn't include any other project headers !!!!
#include <cstdint>

#ifndef BUILDING_TT_SERVER
#ifdef _WIN32
#define PLUGIN_API extern "C" __declspec(dllexport)
#endif
#endif

enum class ArgType
{
    kBool,
    kU8,
    kI8,
    kU16,
    kI16,
    kU32,
    KI32,
    kU64,
    kI64,
    kF32
};

class ScriptFunctionContext
{
  public:
    ScriptFunctionContext(uint32_t count)
    {
        m_pArgTypeStack = (ArgType*)malloc(count * sizeof(ArgType));
        m_pArgStack = (uint8_t*)malloc(1024);
        m_argofs = 0;
    }

    ArgType Type() const
    {
        return m_pArgTypeStack[m_ArgCount];
    }

    ArgType argAt(int i) const
    {
        return m_pArgTypeStack[i];
    }

    void PushAT(ArgType a)
    {
        m_pArgTypeStack[m_ArgCount] = a;
    }

    void Push(bool b)
    {
        PushAT(ArgType::kBool);
        *reinterpret_cast<bool*>(&m_pArgStack[m_argofs]) = b;
        m_argofs += sizeof(bool);
        m_ArgCount++;
    }

    bool PopBool()
    {
        bool b = *reinterpret_cast<bool*>(&m_pArgStack[m_argofs]);
        m_argofs -= sizeof(bool);
        m_ArgCount--;
        return b;
    }

    void Push(float f)
    {
        PushAT(ArgType::kF32);
        *reinterpret_cast<float*>(&m_pArgStack[m_argofs]) = f;
        m_argofs += sizeof(float);
        m_ArgCount++;
    }

    float PopF32()
    {
        float b = *reinterpret_cast<float*>(&m_pArgStack[m_argofs]);
        m_argofs -= sizeof(float);
        m_ArgCount--;
        return b;
    }

    void Push(uint32_t u)
    {
    }

    void Push(int32_t i)
    {
    }

#if 0
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
#endif

    inline uint32_t count() const
    {
        return m_ArgCount;
    }

  private:
    uint8_t* getArgBuffer()
    {
        return &m_pArgStack[m_ArgCount];
    }

  protected:
    uint8_t* m_pReturnValues{nullptr};
    uint8_t* m_pArgStack{nullptr};
    ArgType* m_pArgTypeStack{nullptr};
    uint32_t m_ArgCount{0};
    uint32_t m_argofs{0};
};

// abi stable span
template <typename T> class PluginSlice
{
  public:
    explicit constexpr PluginSlice(const T* ptr, size_t len) : m_pData(ptr), m_size(len)
    {
    }

    template <size_t N> constexpr PluginSlice(T (&a)[N]) noexcept : PluginSlice(a, N)
    {
    }

    constexpr T* data() const
    {
        return m_pData;
    }

  private:
    T* m_pData;
    size_t m_size;
};
using PluginStringView = PluginSlice<const char>;

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
    uint32_t magic;                     // < 'PLGN'
    uint32_t pluginVersion;             // < Version of the plugin (user defined)
    PluginStringView pluginName;        // < Name of the plugin
    PluginStringView authorName;        // < Author name

    enum Flags
    {
        kNone = 0,
        kInternal = 1 << 0,  // < reserved for internal "tilted" use
        kHotReload = 1 << 1, // < plugin supports reloading at runtime
    } flags;                 // < Special plugin flags

    // these are for managing the lifetime of the plugin instance on your own heap.
    PluginInterface001* (*pCreatePlugin)();      // < allocate from your plugin
    void (*pDestroyPlugin)(PluginInterface001*); // < free from your plugin
};
