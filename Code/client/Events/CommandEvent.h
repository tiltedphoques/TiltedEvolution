#pragma once

using TiltedPhoques::String;

/**
* @brief Contains local chat command from the UI to the client.
*/
struct CommandEvent
{
    CommandEvent(String aCommand) 
        : Command(std::move(aCommand))
    {
    }

    String Command{};
};
