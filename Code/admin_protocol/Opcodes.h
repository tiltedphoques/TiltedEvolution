#pragma once

enum ClientAdminOpcode : unsigned char
{
    kAdminShutdown = 0,

    kClientAdminOpcodeMax
};

enum ServerAdminOpcode : unsigned char
{
    kAdminStats = 0,

    kServerAdminOpcodeMax
};
