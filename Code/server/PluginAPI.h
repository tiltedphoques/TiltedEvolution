#pragma once
// WARNING: !!!! This header musn't include any other project headers !!!!
#include <cstdint>

#ifndef BUILDING_TT_SERVER
#ifdef _WIN32
#define PLUGIN_API extern "C" __declspec(dllexport)
#endif
#endif

#ifdef BUILDING_TT_SERVER
#define SERVER_API extern "C" __declspec(dllexport)
#else
#define SERVER_API extern "C" __declspec(dllimport)
#endif

// utilities for sending data to GS
enum class LogLevel
{
    kInfo,
    kDebug
};
SERVER_API void WriteLog(const LogLevel aLogLevel, const char* apFormat, ...);

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

    ArgType GetArg(int i) const { return m_pArgTypeStack[i]; } 

    void Push(bool b) { PushVal(ArgType::kBool, b); }
    void Push(float f) { PushVal(ArgType::kF32, f); }
    // unsigned
    void Push(uint8_t i) { PushVal(ArgType::kU8, i); }
    void Push(uint16_t i) { PushVal(ArgType::kU16, i); }
    void Push(uint32_t i) { PushVal(ArgType::kU32, i); }
    void Push(uint64_t i) { PushVal(ArgType::kU64, i); }
    // signed
    void Push(int8_t i) { PushVal(ArgType::kI8, i); }
    void Push(int16_t i) { PushVal(ArgType::kI16, i); }
    void Push(int32_t i) { PushVal(ArgType::KI32, i); }
    void Push(int64_t i) { PushVal(ArgType::kI64, i); }

    bool PopBool() { return FetchVal<bool>(); }
    bool PopF32() { return FetchVal<float>(); }
    // unsigned
    bool PopU8() { return FetchVal<uint8_t>(); }
    bool PopU16() { return FetchVal<uint16_t>(); }
    bool PopU32() { return FetchVal<uint32_t>(); }
    bool PopU64() { return FetchVal<uint64_t>(); }
    // signed
    bool PopI8() { return FetchVal<int8_t>(); }
    bool PopI16() { return FetchVal<int16_t>(); }
    bool PopI32() { return FetchVal<int32_t>(); }
    bool PopI64() { return FetchVal<int64_t>(); }
    
    inline uint32_t count() const
    {
        return m_ArgCount;
    }

  private:
    template <typename T> void PushVal(const ArgType aType, const T acValue)
    {
        m_pArgTypeStack[m_ArgCount] = aType;
        *reinterpret_cast<T*>(&m_pArgStack[m_argofs]) = acValue;
        m_argofs += sizeof(T);
        m_ArgCount++;
    }

    template <typename T> T FetchVal()
    {
        T value = *reinterpret_cast<T*>(&m_pArgStack[m_argofs]);
        m_argofs -= sizeof(T);
        m_ArgCount--;
        return value;
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
    explicit constexpr PluginSlice(const T* ptr, size_t len) : m_pData(ptr), m_size(len) {}
    template <size_t N> constexpr PluginSlice(T (&a)[N]) noexcept : PluginSlice(a, N) {}

    constexpr T* data() const { return m_pData; }

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
    uint32_t magic;              // < 'PLGN'
    uint32_t structSize;         // < Size of this struct
    uint32_t pluginVersion;      // < Version of the plugin (user defined)
    PluginStringView pluginName; // < Name of the plugin
    PluginStringView authorName; // < Author name

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
