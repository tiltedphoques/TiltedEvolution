
#include "ScriptExecutor.h"

#include <TiltedCore/Buffer.hpp>
#include <spdlog/spdlog.h>

#include "PluginAPI/PluginAPI.h"

ScriptExecutor::ScriptExecutor()
{
}

ScriptExecutor::~ScriptExecutor()
{
}

void ScriptExecutor::RegisterRuntime(const char* const apExtension, PluginAPI::IPluginInterface* apInterface)
{
    auto it = std::find_if(m_scriptRuntimes.begin(), m_scriptRuntimes.end(),
                           [apExtension](const auto& entry) { return entry.first == apExtension; });
    if (it != m_scriptRuntimes.end())
    {
        spdlog::error("Failed to register script extension {}, as a runtime for it already exists");
        return;
    }

    m_scriptRuntimes[apExtension] = apInterface;
}

PluginAPI::IPluginInterface* ScriptExecutor::SelectRuntimeForExtension(const std::string_view acExtension)
{
    auto it = std::find_if(m_scriptRuntimes.begin(), m_scriptRuntimes.end(),
                           [acExtension](const auto& entry) { return entry.first == acExtension; });
    if (it == m_scriptRuntimes.end())
    {
        spdlog::error("Failed to find a runtime for script extension {}", acExtension);
        return nullptr;
    }

    return it->second;
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
    if (auto* pRuntime = SelectRuntimeForExtension(pathExt.string()))
    {
        if (auto* p001 = CastInterfaceVersion<PluginAPI::PluginInterface001>(pRuntime, 1))
        {
            // for now..
            auto pathStr = aPath.string();
            p001->ExecuteFile(PluginAPI::StringRef(pathStr.c_str(), pathStr.length()));
            return;
        }
    }

    spdlog::error("Failed to find applicable runtime interface for file {}", aPath.string());
}

void ScriptExecutor::Bind(const char* const apSymbolName)
{
}
