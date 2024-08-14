#pragma once

enum ChatMessageType : unsigned char
{
    kSystemMessage = 0,
    kGlobalChat,
    kPlayerDialogue,
    kPartyChat,
    kLocalChat
};
