
#include "ScriptExecutor.h"

#include <spdlog/spdlog.h>
#include <TiltedCore/Buffer.hpp>

#include "PluginAPI/PluginAPI.h"

ScriptExecutor::ScriptExecutor()
{
}

ScriptExecutor::~ScriptExecutor()
{
}

void ScriptExecutor::RegisterRuntime(const char* const apExtension, IPluginInterface* apInterface)
{
    m_scriptRuntimes[apExtension] = apInterface;
}

void ScriptExecutor::Execute(const char* aScript)
{
}

void ScriptExecutor::ExecuteFile(const std::filesystem::path& aPath, const Resources::Manifest001& aManifest)
{
    if (!std::filesystem::exists(aPath))
    {
        spdlog::error("Entry point for script does not exist on disk: {}", aPath.string());
        return;
    }

    auto pathExt = aPath.extension();

    IPluginInterface* pInterface = nullptr;
    for (const auto& runtime : m_scriptRuntimes)
    {
        if (pathExt == runtime.first)
        {
            pInterface = runtime.second;
            break;
        }
    }

    if (!pInterface)
    {
        spdlog::error("Failed to find a runtime for file {} with extension: {}", aPath.string(), pathExt.string());
        return;
    }

    if (pInterface->GetVersion() == 1)
    {
        auto* pInterface001 = reinterpret_cast<PluginInterface001*>(pInterface);
        //pInterface001->ExecuteFile(aPath.string().c_str());
    }
    else
        spdlog::error("Failed to find applicable runtime interface for file {}", aPath.string());
}

void ScriptExecutor::Bind(const char* const apSymbolName)
{
}
