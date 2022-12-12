
#include "ScriptExecutor.h"

#include <TiltedCore/Buffer.hpp>
#include <spdlog/spdlog.h>

#include "PluginAPI/PluginAPI.h"

ScriptExecutor::ScriptExecutor() {}

ScriptExecutor::~ScriptExecutor() {}

void ScriptExecutor::RegisterRuntime(const char* const apExtension, PluginAPI::IPluginInterface* apInterface)
{
    auto it = std::find_if(m_scriptExtToPlugin.begin(), m_scriptExtToPlugin.end(), [apExtension](const auto& entry) { return entry.first == apExtension; });
    if (it != m_scriptExtToPlugin.end())
    {
        spdlog::error("Failed to register script extension {}, as a runtime for it already exists");
        return;
    }

    m_scriptExtToPlugin[apExtension] = apInterface;
}

PluginAPI::IPluginInterface* ScriptExecutor::SelectRuntimeForExtension(const std::string_view acExtension)
{
    auto it = std::find_if(m_scriptExtToPlugin.begin(), m_scriptExtToPlugin.end(), [acExtension](const auto& entry) { return entry.first == acExtension; });
    if (it == m_scriptExtToPlugin.end())
    {
        spdlog::error("Failed to find a runtime for script extension {}", acExtension);
        return nullptr;
    }

    return it->second;
}

void ScriptExecutor::LoadFile(const std::filesystem::path& aPath, const Resources::Manifest001& aManifest)
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
            auto pathStr = aPath.string();

            auto scriptHandle = p001->LoadFile(PluginAPI::StringRef(pathStr.c_str(), pathStr.length()));
            if (scriptHandle == 0)
            {
                spdlog::error("Failed to load script file {}", aPath.string());
                return;
            }

            m_handles.push_back(scriptHandle);
            return;
        }
    }

    spdlog::error("Failed to find applicable runtime interface for file {}", aPath.string());
}

void ScriptExecutor::BindMethod(const std::string_view acName, const PluginAPI::ArgType* apArgList, size_t aArgCount, PluginAPI::MethodHandler apCallback)
{
    // TODO: this doesnt make too much sense right now.
    for (const auto& c : m_scriptExtToPlugin)
    {
        if (auto* p001 = CastInterfaceVersion<PluginAPI::PluginInterface001>(c.second, 1))
        {
            for (auto h : m_handles)
            {
                p001->BindMethod(h, PluginAPI::StringRef(acName.data(), acName.length()), apArgList, aArgCount, apCallback);
            }
        }
    }
}

void ScriptExecutor::StartScripts()
{
    const PluginAPI::ArgType Types[] = {PluginAPI::ArgType::kBool, PluginAPI::ArgType::kBool};
    BindMethod(
        "test_func", Types, 2,
        [](PluginAPI::ActionStack& acContext)
        {
            auto a = acContext.PopBool();
            auto b = acContext.PopBool();
            spdlog::info("Context matters is {}>{}", a, b);
        });

    for (const auto& c : m_scriptExtToPlugin)
    {
        if (auto* p001 = CastInterfaceVersion<PluginAPI::PluginInterface001>(c.second, 1))
        {
            for (auto h : m_handles)
            {
                PluginAPI::ActionStack stack(0);
                p001->CallMethod(h, "plugin_main", stack);
            }
        }
    }
}
