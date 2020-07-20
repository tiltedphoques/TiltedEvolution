#pragma once

enum ClientOpcode : unsigned char
{
    kAuthenticationRequest = 0,
    kCancelAssignmentRequest,
    kRemoveCharacterRequest,
    kAssignCharacterRequest,
    kClientReferencesMoveRequest,
    kEnterCellRequest
};

enum ServerOpcode : unsigned char
{
    kAuthenticationResponse = 0,
    kAssignCharacterResponse,
    kServerReferencesMoveRequest
};
