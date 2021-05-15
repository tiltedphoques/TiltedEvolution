#pragma once

enum ClientOpcode : unsigned char
{
    kAuthenticationRequest = 0,
    kCancelAssignmentRequest,
    kAssignCharacterRequest,
    kClientReferencesMoveRequest,
    kEnterExteriorCellRequest,
    kEnterInteriorCellRequest,
    kClientRpcCalls,
    kRequestInventoryChanges,
    kRequestFactionsChanges,
    kRequestQuestUpdate,
    kPartyInviteRequest,
    kPartyAcceptInviteRequest,
    kPartyLeaveRequest,
    kRequestActorValueChanges,
    kRequestActorMaxValueChanges,
    kRequestHealthChangeBroadcast,
    kActivateRequest,
    kLockChangeRequest,
    kAssignObjectsRequest,
    kRequestSpawnData,
    kRequestDeathStateChange,
    kShiftGridCellRequest,
    kRequestOwnershipTransfer,
    kRequestOwnershipClaim,
    kClientOpcodeMax
};

enum ServerOpcode : unsigned char
{
    kAuthenticationResponse = 0,
    kAssignCharacterResponse,
    kServerReferencesMoveRequest,
    kServerScriptUpdate,
    kServerTimeSettings,
    kCharacterSpawnRequest,
    kNotifyInventoryChanges,
    kNotifyFactionsChanges,
    kNotifyRemoveCharacter,
    kNotifyQuestUpdate,
    kNotifyPlayerList,
    kNotifyPartyInfo,
    kNotifyPartyInvite,
    kNotifyActorValueChanges,
    kNotifyActorMaxValueChanges,
    kNotifyHealthChangeBroadcast,
    kNotifySpawnData,
    kNotifyActivate,
    kNotifyLockChange,
    kAssignObjectsResponse,
    kNotifyDeathStateChange,
    kNotifyOwnershipTransfer,
    kServerOpcodeMax
};
