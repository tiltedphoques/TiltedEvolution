#pragma once

using TiltedPhoques::String;

struct CommandEvent
{
    CommandEvent(String aCommand) 
        : Command(std::move(aCommand))
    {
    }

    String Command{};
};
