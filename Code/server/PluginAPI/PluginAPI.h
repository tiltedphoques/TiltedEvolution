// Copyright (C) 2022 TiltedPhoques SRL.
// For licensing information see LICENSE at the root of this distribution.
#pragma once

// WARNING: !!!! This header musn't include any other project headers !!!!

#include <cstdint>
#include <cstdio>  /* printf, scanf, NULL */
#include <cstdlib> /* malloc, free, rand */

#ifndef BUILDING_TT_SERVER
#ifdef _WIN32
#define PLUGIN_API extern "C" __declspec(dllexport)
#endif

#ifdef __linux__
#define PLUGIN_API extern "C" __attribute__((visibility("default")))
#endif
#endif

#ifdef BUILDING_TT_SERVER

#ifdef _WIN32
#define SERVER_API extern "C" __declspec(dllexport)
#endif

#ifdef __linux__
#define SERVER_API extern "C" __attribute__((visibility("default")))
#endif

#else

#ifdef _WIN32
#define SERVER_API extern "C" __declspec(dllimport)
#endif

#ifdef __linux__
#define SERVER_API extern "C" __attribute__((visibility("default")))
#endif

#endif

#include "Action.h"
#include "Slice.h"

namespace PluginAPI
{
static constexpr uint32_t kPluginMagic = 'PLGN';
static constexpr uint32_t kMaxPluginInterfaceVersion = 1;

struct InfoBlock
{
    uint32_t magic;
    uint32_t structSize;
    void* ptr;
};
static_assert(sizeof(InfoBlock) == 16, "InfoBlock size mismatch");

struct ScriptInfoBlock
{
    static constexpr uint32_t kMagic = 'SINF';
    int supportedExtensionCount;
    const char** supportedExtensions;
};
static_assert(sizeof(ScriptInfoBlock) == 16, "ScriptInfoBlock size mismatch");

class IPluginInterface;

struct PluginDescriptor
{
    // make sure to not reorder those first two!!!
    uint32_t magic;      // < 'PLGN'
    uint32_t structSize; // < Size of this struct

    uint32_t version; // < Version of the plugin (user defined)
    StringRef name;   // < Name of the plugin
    StringRef author; // < Author name
    InfoBlock infoblocks[2]{};

    enum Flags : uint32_t
    {
        kFlagNone = 0,
        kFlagInternal = 1 << 0,  // < reserved for internal "tilted" use
        kFlagHotReload = 1 << 1, // < plugin supports reloading at runtime
    };
    uint32_t flags;

    bool CanHotReload() const noexcept
    {
        return (flags & Flags::kFlagHotReload) != 0;
    }

    bool IsSpecialPlugin() const noexcept
    {
        return (flags & Flags::kFlagInternal) != 0;
    }

    enum Entitlements : uint32_t
    {
        kEntNone = 0,
        kEntScripting = 1 << 0, // < plugin wants to support scripting
        kEntUpdate = 1 << 1,    // < plugin wants to receive update events
    };
    uint32_t entitlements;

    bool IsScriptPlugin() const noexcept
    {
        return (entitlements & Entitlements::kEntScripting) != 0;
    }

    // these are for managing the lifetime of the plugin instance on your own heap.
    IPluginInterface* (*pCreatePlugin)();      // < allocate from your plugin
    void (*pDestroyPlugin)(IPluginInterface*); // < free from your plugin
};
// constexpr auto x = sizeof(PluginDescriptor);
static_assert(sizeof(PluginDescriptor) == 104, "PluginDescriptor size mismatch");

// the maximum plugin interface version possible at the moment!
static constexpr uint32_t kCurrentPluginInterfaceVersion = 1;

// IPluginInterface implements a bare standard, 001 adds scripting support
// if you want to add a new interface version, inherit from the base class, and implement new functionality
// e.g. PluginInterface002 : IPluginInterface
class IPluginInterface
{
  public:
    virtual ~IPluginInterface() = default;

    virtual uint32_t GetVersion() = 0;

    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;

    // only called if the plugin has the kUpdate entitlement
    virtual void OnTick() = 0;

    // these are not enum class, cause they might be extended by newer interface versions!
    enum EventCode : uint32_t
    {
        kEventCodePreHotReload,
        kEventCodePreShutdown,
    };
    enum EventResult : uint32_t
    {
        kEventResultContinue,
        kEventResultStop,
    };
    virtual uint32_t OnEvent(uint32_t aEventCode)
    {
        return kEventResultContinue;
    }
};

// maybe we just do a type inheritance instead??
// so ScriptPlugin : IPluginInterface..
class PluginInterface001 : public IPluginInterface
{
  public:
    virtual ~PluginInterface001() = default;

    virtual uint32_t GetVersion() override
    {
        return 1;
    }

    // only called if the plugin has the kScripting entitlement
    virtual void BindAction(const StringRef acActionName, const ArgType* args, size_t argCount,
                            void(*aCallback)(ActionStack& acContext)){};
    virtual void InvokeAction(const StringRef acActionName, ActionStack& acStack){};

    virtual void ExecuteCode(const StringRef acCode){};
    virtual void ExecuteFile(const StringRef acFileName){};
};
} // namespace PluginAPI
