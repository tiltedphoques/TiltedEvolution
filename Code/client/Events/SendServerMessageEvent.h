#pragma once

struct TESObjectREFR;

struct SendServerMessageEvent
{
    SendServerMessageEvent(ClientMessage message) : Message(message)
    {}

    ClientMessage Message;
};
