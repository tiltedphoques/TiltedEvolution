#pragma once

using TiltedPhoques::String;

/**
* @brief Dispatched when the game code registers a Papyrus function internally.
*/
struct PapyrusFunctionRegisterEvent
{
    explicit PapyrusFunctionRegisterEvent(const char* acpFunctionName, const char* acpNamespace, void* apFunctionAddress)
        : Name(acpFunctionName)
        , Namespace(acpNamespace)
        , Function(apFunctionAddress)
    {}

    String Name;
    String Namespace;
    void* Function;
};
