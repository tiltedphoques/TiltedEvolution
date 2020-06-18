#pragma once

enum ClientOpcode : unsigned char
{
    kAuthenticationRequest = 0,
};

enum ServerOpcode : unsigned char
{
    kAuthenticationResponse = 0,
};
