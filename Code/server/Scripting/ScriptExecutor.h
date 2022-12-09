#pragma once

#include <resources/Manifest.h>
#include <TiltedCore/Platform.hpp>

class IPluginInterface;

class ScriptExecutor
{
  public:
    ScriptExecutor();
    ~ScriptExecutor();
    
    void RegisterRuntime(const char* const apExtension, IPluginInterface* apInterface);

    void Execute(const char* aScript);
    void ExecuteFile(const std::filesystem::path& aPath, const Resources::Manifest001& aManifest);

    void Bind(const char* const apSymbolName);

  private:
    TiltedPhoques::Map<TiltedPhoques::String, IPluginInterface*> m_scriptRuntimes;
};
