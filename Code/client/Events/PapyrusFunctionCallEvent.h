#pragma once

struct PapyrusFunctionCallEvent
{
    struct Argument
    {
        String Name{};
        String Type{};
        std::optional<std::any> Value{};
    };

    uint32_t StackId{};
    String ScriptName{};
    String FunctionName{};
    uint32_t SelfFormId{};
    Vector<Argument> Arguments{};
};
