#pragma once

#include <TiltedCore/Platform.hpp>
#include <resources/Manifest.h>

#include "PluginAPI/PluginAPI.h"
#include "PluginAPI/SmallFunction.h"

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

    template <typename ...Ts>
    void CallFunction(const std::string_view acName, Ts ... args)
    {
        PluginAPI::ActionStack stack(sizeof...(Ts));
        stack.PushN(std::forward<Ts>(args)...);
        CallMethod(acName, stack);
    }
    
    template <typename R, typename... Ts>
    void FancyBind(const std::string_view acName, PluginAPI::SmallFunction<R, Ts...> functor)
    {
        const PluginAPI::ArgType array[] = {(PluginAPI::ToArgType<Ts>(), ...)};
        //BindMethod(acName, &array[0], sizeof...(Ts), functor);
    }


    void StartScripts();

  private:
      // call a method
    void CallMethod(const std::string_view acName, PluginAPI::ActionStack& aStack);
    
    // bind a function to all scripting runtimes.
    void BindMethod(const std::string_view acName, const PluginAPI::ArgType* apArgList, size_t aArgCount,
                    PluginAPI::MethodHandler apCallback);

  private:
    PluginAPI::IPluginInterface* SelectRuntimeForExtension(const std::string_view acExtension);

    // check if the feature level is supported
    template <typename T> T* CastInterfaceVersion(PluginAPI::IPluginInterface* apInterface, int version)
    {
        return apInterface->GetVersion() == version ? reinterpret_cast<T*>(apInterface) : nullptr;
    }

  private:
    TiltedPhoques::Map<TiltedPhoques::String, PluginAPI::IPluginInterface*> m_scriptExtToPlugin;

    struct HandleRef
    {
        uint32_t handle;
        union {
            uint8_t plugin : 8;
            uint8_t module : 8;
            uint16_t internal : 16;
        };
    };
    TiltedPhoques::Vector<HandleRef> m_handles;
};
