#pragma once

enum ClientOpcode : unsigned char
{
    kAuthenticationRequest = 0,
    kCancelAssignmentRequest,
    kRemoveCharacterRequest
};

enum ServerOpcode : unsigned char
{
    kAuthenticationResponse = 0,
};
