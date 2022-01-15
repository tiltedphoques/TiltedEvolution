#pragma once

struct TESObjectREFR;

struct SendServerMessageEvent
{
    SendServerMessageEvent(ClientMessage aMessage) : Message(aMessage)
    {}

    ClientMessage Message;
};
