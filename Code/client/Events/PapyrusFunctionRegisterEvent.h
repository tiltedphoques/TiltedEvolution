#pragma once

#include <Stl.hpp>

using TiltedPhoques::String;

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
