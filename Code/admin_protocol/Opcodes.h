#pragma once

enum ClientAdminOpcode : unsigned char
{
    kAdminShutdown = 0,

    kClientAdminOpcodeMax
};

enum ServerAdminOpcode : unsigned char
{
    kAdminSessionOpen = 0,
    kServerLogs,

    kServerAdminOpcodeMax
};
