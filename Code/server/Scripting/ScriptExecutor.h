#pragma once

#include <TiltedCore/Platform.hpp>
#include <resources/Manifest.h>

#include "PluginAPI/PluginAPI.h"

class ScriptExecutor
{
  public:
    ScriptExecutor();
    ~ScriptExecutor();

    // register a runtime extension such as .py for python and a runtime that serves requests for files that contain the
    // extension
    void RegisterRuntime(const char* const apExtension, PluginAPI::IPluginInterface* apInterface);

    // Load the script file at the given path and setup up runtime stuff surrounding it.
    void LoadFile(const std::filesystem::path& aPath, const Resources::Manifest001& aManifest);

    // bind a function to all scripting runtimes.
    void BindMethod (const std::string_view acName, const PluginAPI::ArgType* apArgList, size_t aArgCount,
              PluginAPI::ActionCallback apCallback);

    void StartScripts();
    //template <typename... Ts> void FancyBind(const std::string_view acName, Ts...);

  private:
    PluginAPI::IPluginInterface* SelectRuntimeForExtension(const std::string_view acExtension);
    
    // check if the feature level is supported
    template <typename T> T* CastInterfaceVersion(PluginAPI::IPluginInterface* apInterface, int version)
    {
        return apInterface->GetVersion() == version ? reinterpret_cast<T*>(apInterface) : nullptr;
    }

  private:
    TiltedPhoques::Map<TiltedPhoques::String, PluginAPI::IPluginInterface*> m_scriptExtToPlugin;
    TiltedPhoques::Vector<PluginAPI::PluginInterface001::Handle> m_handles;
};
